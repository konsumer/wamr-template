// this implements the host-specific API utils & callbacks for wamr

#pragma once

#include <time.h>

// get current unix-time in ms
uint64_t null0_millis() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

cvector_vector_type(NativeSymbol) null0_native_symbols = NULL;

static wasm_function_inst_t cart_update = NULL;
static wasm_function_inst_t cart_unload = NULL;
static wasm_function_inst_t cart_buttonUp = NULL;
static wasm_function_inst_t cart_buttonDown = NULL;
static wasm_function_inst_t cart_keyUp = NULL;
static wasm_function_inst_t cart_keyDown = NULL;

// used to call update
static u32 update_args[2];

// these will be used in callbacks & wasm_host_load_wasm
static wasm_module_t module;
static wasm_module_inst_t module_inst;
static wasm_exec_env_t null0_exec_env;

// implement these memory-helpers for each host

// copy a host-pointer to cart when you already have a cart-pointer
void copy_to_cart_with_pointer(u32 cartPtr, void* hostPtr, u32 size) {
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr);
  memcpy(cartHostPtr, hostPtr, size);
}

// copy a host-pointer to cart when you already have a cart-pointer
void copy_from_cart_with_pointer(void* hostPtr, u32 cartPtr, u32 size) {
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr);
  memcpy(hostPtr, cartHostPtr, size);
}

// allocate some memory in cart
u32 cart_malloc(u32 size) {
  void** p_native_addr = NULL;
  return (u32)wasm_runtime_module_malloc(module_inst, (u64)size, p_native_addr);
}

// free some memory in cart
void cart_free(u32 ptr) {
  wasm_runtime_module_free(module_inst, (u64)ptr);
}


// get the strlen of a cart-pointer
u32 cart_strlen(u32 cartPtr) {
  return strlen(wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr));
}


// this has the actual cross-host API implementation
#include "host_api.h"

// implement these callbacks for each host

// called on native wamr/web host to load actual cart bytes expose host-functions to it
bool wasm_host_load_wasm(u8* wasmBytes, u32 wasmBytesLen) {
  wasm_runtime_init();
  
  if (!wasm_runtime_register_natives("null0", null0_native_symbols, cvector_size(null0_native_symbols))) {
    return false;
  }
  
  char error_buf[128];
  wasm_module_t module = wasm_runtime_load(wasmBytes, wasmBytesLen, error_buf, sizeof(error_buf));
  module_inst = wasm_runtime_instantiate(module, 65536, 0, error_buf, sizeof(error_buf));
  null0_exec_env = wasm_runtime_create_exec_env(module_inst, 32768);

  wasm_function_inst_t cart_load = wasm_runtime_lookup_function(module_inst, "load");
  cart_unload = wasm_runtime_lookup_function(module_inst, "unload");
  cart_update = wasm_runtime_lookup_function(module_inst, "update");
  cart_buttonUp = wasm_runtime_lookup_function(module_inst, "buttonUp");
  cart_buttonDown = wasm_runtime_lookup_function(module_inst, "buttonDown");
  cart_keyUp = wasm_runtime_lookup_function(module_inst, "keyUp");
  cart_keyDown = wasm_runtime_lookup_function(module_inst, "keyDown");

  wasm_application_execute_main(module_inst, 0, NULL);

  if (cart_load != NULL) {
    if (!wasm_runtime_call_wasm(null0_exec_env, cart_load, 0, NULL)) {
      // not fatal, but this will help with troubleshooting
      printf("load: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }

  return true;
}

// called on each frame
void wasm_host_update() {
  if (cart_update != NULL) {
    u64 t = null0_millis();
    update_args[0] = (u32)t;
    update_args[1] = (u32)(t >> 32);
    if (!wasm_runtime_call_wasm(null0_exec_env, cart_update, 2, update_args)) {
      fprintf(stderr, "update: %s\n", wasm_runtime_get_exception(module_inst));
    }
  }
}

// called when cart is unloaded
void wasm_host_unload_wasm(){}
