// this is the emscripten-only host memory helpers

#pragma once

#include "emscripten.h"

// copy a host-pointer to cart, return cart-pointer
EM_JS(unsigned int, copy_to_cart, (void* hostPtr, unsigned int size), {
  const outPtr = Module.cart.malloc(size);
  new Uint8Array(Module.cart.memory.buffer).set(Module.HEAPU8.slice(hostPtr, hostPtr+size), outPtr);
  return outPtr;
})

// copy a host-pointer to cart when you already have a cart-pointer
EM_JS(void, copy_to_cart_with_pointer, (unsigned int cartPtr, void* hostPtr, unsigned int size), {
  new Uint8Array(Module.cart.memory.buffer).set(Module.HEAPU8.slice(hostPtr, hostPtr+size), cartPtr);
})

// get the strlen of a cart-pointer
EM_JS(int, cart_strlen, (unsigned int cartPtr), {
  return Module.cart_strlen(cartPtr);
});

// copy a cart-pointer to host when you already have a host-pointer
EM_JS(void, copy_from_cart_with_pointer, (void* hostPtr, unsigned int cartPtr, unsigned int size), {
  Module.HEAPU8.set(new Uint8Array(Module.cart.memory.buffer).slice(cartPtr, cartPtr+size), hostPtr);
})
