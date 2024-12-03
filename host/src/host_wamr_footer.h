// this is the wamr-only host callbacks

#pragma once

#include <time.h>

static wasm_function_inst_t cart_update = NULL;
static wasm_function_inst_t cart_unload = NULL;
static wasm_function_inst_t cart_buttonUp = NULL;
static wasm_function_inst_t cart_buttonDown = NULL;
static wasm_function_inst_t cart_keyUp = NULL;
static wasm_function_inst_t cart_keyDown = NULL;

// used to call update
static uint64_t update_args[1];


// get current unix-time in ms
uint64_t null0_millis() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

// called on native wamr/web host to load actual cart bytes expose host-functions to it
bool wasm_host_load_wasm (unsigned char* wasmBytes, uint32_t wasmBytesLen) {

  RuntimeInitArgs init_args;
  memset(&init_args, 0, sizeof(RuntimeInitArgs));

  init_args.mem_alloc_type = Alloc_With_Pool;
  static char global_heap_buf[8092 * 8];
  init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
  init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

  init_args.n_native_symbols = cvector_size(null0_native_symbols);
  init_args.native_module_name = "null0";
  init_args.native_symbols = null0_native_symbols;

  if (!wasm_runtime_full_init(&init_args)) {
    fprintf(stderr, "ERROR: Init runtime environment failed.\n");
    return false;
  }

  uint32_t stack_size = 8092;
  uint32_t heap_size = 8092;
  char error_buf[128];

  error_buf[0] = 0;
  module = wasm_runtime_load(wasmBytes, wasmBytesLen, error_buf, 128);
  free(wasmBytes);

  if (error_buf[0] != 0) {
    fprintf(stderr, "ERROR: load - %s\n", error_buf);
    return false;
  }

  error_buf[0] = 0;
  module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, 128);
  if (error_buf[0] != 0) {
    fprintf(stderr, "ERROR: instantiate - %s\n", error_buf);
    return false;
  }

  exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);

  wasm_function_inst_t cart_load = NULL;

  cart_load = wasm_runtime_lookup_function(module_inst, "load");
  cart_unload = wasm_runtime_lookup_function(module_inst, "unload");
  cart_update = wasm_runtime_lookup_function(module_inst, "update");
  cart_buttonUp = wasm_runtime_lookup_function(module_inst, "buttonUp");
  cart_buttonDown = wasm_runtime_lookup_function(module_inst, "buttonDown");
  cart_keyUp = wasm_runtime_lookup_function(module_inst, "keyUp");
  cart_keyDown = wasm_runtime_lookup_function(module_inst, "keyDown");

  // for some reason load mustbe called before main

  if (cart_load != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_load, 0, NULL)) {
      // not fatal, but this will help with troubleshooting
      printf("load: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }

  wasm_application_execute_main(module_inst, 0, NULL);

  return true;
}

// called when cart is unloaded
void wasm_host_unload() {
  // TODO
}

// called on each frame
void wasm_host_update() {
  if (cart_update != NULL) {
    update_args[0] = null0_millis();
    wasm_runtime_call_wasm(exec_env, cart_update, 2, (uint32_t*)update_args);
  }
}
