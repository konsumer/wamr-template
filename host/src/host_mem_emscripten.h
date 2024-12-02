// this is the emscripten-only host memory helpers

#pragma once

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
  const MAX_STR_LEN=1024;
  let len=0;
  const mem = new Uint8Array(Module.cart.memory.buffer.slice(cartPtr, cartPtr + MAX_STR_LEN));
  for (len=0;len<MAX_STR_LEN;len++) {
    if (mem[len] === 0) {
      break;
    }
  }
  if (len === MAX_STR_LEN) {
    return -1;
  }
  return len;
});
