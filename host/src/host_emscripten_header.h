// this is the emscripten-only host memory helpers

#pragma once

#include "emscripten.h"

// copy a host-pointer to cart, return cart-pointer
EM_JS(unsigned int, copy_to_cart, (void* hostPtr, unsigned int size), {
  const outPtr = Module.cart.malloc(size);
  new Uint8Array(Module.cart.memory.buffer).set(Module.HEAPU8.slice(hostPtr, hostPtr+size), outPtr);
  return outPtr;
})

// copy a host-pointer to cart whenb you already have a cart-pointer
EM_JS(void, copy_to_cart_with_pointer, (unsigned int outPtr, void* hostPtr, unsigned int size), {
  new Uint8Array(Module.cart.memory.buffer).set(Module.HEAPU8.slice(hostPtr, hostPtr+size), outPtr);
})

EM_JS(void, _wasm_host_copy_from_cart, (unsigned int cartPtr, void* hostPtr, unsigned int size), {
  let i = 0;
  const mem = new Uint8Array(Module.cart.memory.buffer.slice(cartPtr, cartPtr+size));
  for (i=0;i<size;i++) {
    Module.HEAPU8[hostPtr + i] = mem[i]
  }
});

// copy a host-pointer to cart, return cart-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void *out = malloc(size);
  _wasm_host_copy_from_cart(cartPtr, out, size);
  return out;
}

// get the strlen of a cart-pointer
EM_JS(int, cart_strlen, (unsigned int cartPtr), {
  return Module.cart_strlen(cartPtr);
});
