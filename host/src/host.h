#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wasm_c_api.h"
#include "wasm_export.h"

typedef struct {
  unsigned int x;
  unsigned int y;
} Point;

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

static int millis() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return ((unsigned int)now.tv_sec) * 1000 + ((unsigned int)now.tv_nsec) / 1000000;
}

// host string param
static void test_string_in(wasm_exec_env_t exec_env, char* i) {
  printf("host: string from cart: %s\n", i);
}

// host string return
static void test_string_out(wasm_exec_env_t exec_env, unsigned int outPtr) {
  char* o = "ok";
  unsigned char* bufferOut = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)outPtr);
  memcpy(bufferOut, o, strlen(o));
}

// host bytes param (with length)
static void test_bytes_in(wasm_exec_env_t exec_env, unsigned int* i, unsigned int iLen) {}

// host bytes return (with length)
static void test_bytes_out(wasm_exec_env_t exec_env, unsigned int outPtr, unsigned int lenPtr) {
  unsigned char* bufferOut = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)outPtr);
  unsigned char* lenOut = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)lenPtr);

  unsigned int len = 4;
  unsigned char bytes[4] = {1, 2, 3, 4};
  memcpy(bufferOut, bytes, len);
  memcpy(lenOut, &len, sizeof(len));
}

// host struct param (using length to make copy work)
static void test_struct_in(wasm_exec_env_t exec_env, Point* i, unsigned int iLen) {
  printf("host: cart sent point %ux%u\n", i->x, i->y);
}

// host struct return (returns length)
static void test_struct_out(wasm_exec_env_t exec_env, unsigned int o) {
  unsigned char* bufferOut = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)o);
  Point r = {100, 200};
  memcpy(bufferOut, &r, sizeof(r));
}

static NativeSymbol native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(test_string_in, "($)"),
    EXPORT_WASM_API_WITH_SIG(test_string_out, "(i)"),
    EXPORT_WASM_API_WITH_SIG(test_bytes_in, "(*~)"),
    EXPORT_WASM_API_WITH_SIG(test_bytes_out, "(ii)"),
    EXPORT_WASM_API_WITH_SIG(test_struct_in, "(*~)"),
    EXPORT_WASM_API_WITH_SIG(test_struct_out, "(i)")};

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
    func_args[0].of.i32 = millis();
    if (!wasm_runtime_call_wasm_a(exec_env, cart_update, 0, NULL, 1, func_args)) {
      printf("update failed: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }
}
