// this is the emscripten-only host memory helpers

EM_JS(void, _wasm_host_copy_from_cart, (unsigned int cartPtr, void* hostPtr, unsigned int size), {
  let i = 0;
  const mem = new Uint8Array(Module.cart.exports.memory.buffer.slice(cartPtr, cartPtr+size));
  for (i=0;i<size;i++) {
    Module.HEAPU8[hostPtr + i] = mem[i]
  }
});

// Copy a pointer from cart to host
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void *out = malloc(size);
  _wasm_host_copy_from_cart(cartPtr, out, size);
  return out;
}

// Get the length of a cart string
EM_JS(int, cart_strlen, (unsigned int cartPtr), {
  const MAX_STR_LEN=1024;
  let len=0;
  const mem = new Uint8Array(Module.cart.exports.memory.buffer.slice(cartPtr, cartPtr + MAX_STR_LEN));
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

// Copy from a host pointer to cart, return cart pointer
EM_JS(unsigned int, copy_to_cart, (void* hostPtr, unsigned int size), {
  const cartPtr = Module.cart.exports.malloc(size);
  const cartBytes = Module.HEAPU8.slice(hostPtr, hostPtr + size);
  const mem = new Uint8Array(Module.cart.exports.memory.buffer);
  mem.set(cartBytes, cartPtr);
  return cartPtr;
});
