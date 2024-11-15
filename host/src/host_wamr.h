#include <string.h>
#include <stdlib.h>
#include "wasm_c_api.h"
#include "wasm_export.h"

static char global_heap_buf[512 * 1024];
static wasm_val_t func_args[2];
static wasm_function_inst_t cart_update = NULL;
static wasm_function_inst_t cart_unload = NULL;
static wasm_function_inst_t cart_buttonUp = NULL;
static wasm_function_inst_t cart_buttonDown = NULL;
static wasm_function_inst_t cart_keyUp = NULL;
static wasm_function_inst_t cart_keyDown = NULL;

static wasm_exec_env_t exec_env;
static wasm_module_t module;
static wasm_module_inst_t module_inst;

// copy a pointer from cart to host
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void* out = malloc(size);
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr);
  memcpy(out, cartHostPtr, size);
  return out;
}

// copy a pointer from host to cart
unsigned int copy_to_cart(void* hostPtr, unsigned int size) {
  return (unsigned int) wasm_runtime_module_dup_data(module_inst, (const char*)hostPtr, (uint64_t) size);
}

/// API

// send a string to host
// void test_string_in(char* str);
void test_string_in(wasm_exec_env_t exec_env, char* str) {
  printf("host: test_string_in - %s\n", str);
}

// return a string from host
// char* test_string_out();
unsigned int test_string_out(wasm_exec_env_t exec_env){
  char* str = "hi!";
  return copy_to_cart(str, strlen(str) + 1);
}

// send some bytes to host
// void test_bytes_in(unsigned char* bytes, unsigned int bytesLen);
void test_bytes_in(wasm_exec_env_t exec_env, unsigned char* bytes, unsigned int bytesLen) {
  printf("host: test_bytes_in (%u) - %u %u %u %u\n", bytesLen, bytes[0], bytes[1], bytes[2], bytes[3]);
}

// return some bytes from host
// unsigned char* test_bytes_out(unsigned int* outLen);
unsigned int test_bytes_out(wasm_exec_env_t exec_env, unsigned int* outLen) {
  *outLen = 4;
  unsigned char bytes[] = {0,1,2,3};
  return copy_to_cart(bytes, *outLen);
}

// send struct to host
// void test_struct_in(TestPoint* point);
void test_struct_in(wasm_exec_env_t exec_env,  unsigned int pointPntr) {
  TestPoint* point = copy_from_cart(pointPntr, sizeof(TestPoint));
  printf("host: test_struct_in - %ux%u\n", point->x, point->y);
}

// return struct from host
// TestPoint* test_struct_out();
unsigned int test_struct_out(wasm_exec_env_t exec_env) {
  TestPoint point = {.x=200, .y=100};
  return copy_to_cart(&point, sizeof(point));
}

///

/*
see this: https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md
  - $ or * return-vals don't work, use i (and copy out values)
  - * param is a pointer to i32 (since only 4 bytes are copied automatically) unless you can do *~, use i and copy the bytes yourself
  - use $ params, they work great
*/

static NativeSymbol native_symbols[] = {
  EXPORT_WASM_API_WITH_SIG(test_string_in, "($)"),
  EXPORT_WASM_API_WITH_SIG(test_string_out, "()i"),
  EXPORT_WASM_API_WITH_SIG(test_bytes_in, "(*~)"),
  EXPORT_WASM_API_WITH_SIG(test_bytes_out, "(*)i"),
  EXPORT_WASM_API_WITH_SIG(test_struct_in, "(i)"),
  EXPORT_WASM_API_WITH_SIG(test_struct_out, "()i")
};

int wasm_host_load(char* filename) {
  unsigned char* wasmBytes;

  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("Failed to open wasm file\n");
    return -1;
  }

  fseek(file, 0, SEEK_END);
  unsigned int bytesRead = ftell(file);
  fseek(file, 0, SEEK_SET);

  wasmBytes = malloc(bytesRead);
  fread(wasmBytes, 1, bytesRead, file);
  fclose(file);

  if (wasmBytes == NULL) {
    fprintf(stderr, "ERROR: Could not read main.wasm\n");
    return -1;
  }

  RuntimeInitArgs init_args;
  memset(&init_args, 0, sizeof(RuntimeInitArgs));

  static char global_heap_buf[8092 * 8];
  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
  init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

  init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
  init_args.native_module_name = "null0";
  init_args.native_symbols = native_symbols;

  if (!wasm_runtime_full_init(&init_args)) {
    fprintf(stderr, "ERROR: Init runtime environment failed.\n");
    return -1;
  }

  uint32_t stack_size = 8092, heap_size = 8092;
  unsigned long wasmSize = (unsigned long)bytesRead;
  char error_buf[128];

  error_buf[0] = 0;
  module = wasm_runtime_load(wasmBytes, wasmSize, error_buf, 128);
  if (error_buf[0] != 0) {
    fprintf(stderr, "ERROR: load - %s\n", error_buf);
    return -1;
  }

  error_buf[0] = 0;
  module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, 128);
  if (error_buf[0] != 0) {
    fprintf(stderr, "ERROR: instantiate - %s\n", error_buf);
    return -1;
  }

  exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);

  wasm_function_inst_t cart_load = NULL;

  cart_load = wasm_runtime_lookup_function(module_inst, "load");
  cart_update = wasm_runtime_lookup_function(module_inst, "update");
  cart_buttonUp = wasm_runtime_lookup_function(module_inst, "buttonUp");
  cart_buttonDown = wasm_runtime_lookup_function(module_inst, "buttonDown");
  cart_keyUp = wasm_runtime_lookup_function(module_inst, "keyUp");
  cart_keyDown = wasm_runtime_lookup_function(module_inst, "keyDown");

  if (cart_load != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_load, 0, NULL)) {
      // not fatal, but this will help with troubleshooting
      printf("load: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }

  wasm_application_execute_main(module_inst, 0, NULL);

  return 0;
}

void wasm_host_unload() {
  if (cart_unload != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_unload, 0, NULL)) {
      // not fatal, but this will help with troubleshooting
      printf("unload: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }
  if (exec_env != NULL) {
    wasm_runtime_destroy_exec_env(exec_env);
  }
  if (module_inst != NULL) {
    wasm_runtime_deinstantiate(module_inst);
  }
  if (module != NULL) {
    wasm_runtime_unload(module);
  }
  wasm_runtime_destroy();
}

void wasm_host_update() {
  if (cart_update != NULL) {
    func_args[0].kind = WASM_I32;
    func_args[0].of.i32 = null0_millis();
    if (!wasm_runtime_call_wasm_a(exec_env, cart_update, 0, NULL, 1, func_args)) {
      printf("update failed: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }
}
