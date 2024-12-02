// this is the wamr-only host memory helpers

#pragma once

#include "wasm_c_api.h"
#include "wasm_export.h"

static wasm_module_t module;
static wasm_module_inst_t module_inst;
static wasm_exec_env_t exec_env;

// copy a host-pointer to cart, return cart-pointer
unsigned int copy_to_cart(void* hostPtr, unsigned int size) {
  return (unsigned int) wasm_runtime_module_dup_data(module_inst, (const char*)hostPtr, (uint64_t) size);
}

// copy a host-pointer to cart whenb you already have a cart-pointer
void copy_to_cart_with_pointer(unsigned int outPtr, void* hostPtr, unsigned int size) {
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)outPtr);
  memcpy(cartHostPtr, hostPtr, size);
}

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void* out = malloc(size);
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr);
  memcpy(out, cartHostPtr, size);
  return out;
}

// get the strlen of a cart-pointer
int cart_strlen(unsigned int cartPtr) {
  return strlen(wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr));
}
