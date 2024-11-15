#include "emscripten.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void wasm_host_load(char *filename) {}

void wasm_host_unload() {}

EM_JS(void, wasm_host_update, (), {
  if (Module?.cart?.exports?.update) {
    Module.cart.exports.update(Date.now());
  }
});

EM_JS(void, _wasm_host_copy_from_cart, (unsigned int cartPtr, void* hostPtr, unsigned int size), {
  Module.HEAPU8.set(Module.cart.exports.memory.buffer.slice(cartPtr, cartPtr + size), hostPtr);
});

// copy a pointer from cart to host
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void *out = malloc(size);
  _wasm_host_copy_from_cart(cartPtr, out, size);
  return out;
}

EM_JS(int, cart_strlen, (unsigned int cartPtr), {
  const MAX_STR_LEN=1024;
  let len=0;
  for (len=0;len<MAX_STR_LEN;len++) {
    if (Module.HEAPU8[cartPtr + len] === 0) {
      break;
    }
  }
  if (len === MAX_STR_LEN) {
    return -1;
  }
  return len;
});

// copy a pointer to a string from cart to host
char* copy_from_cart_string(unsigned int cartPtr) {
  int len = cart_strlen(cartPtr);
  char* out = NULL;
  if (len) {
    out = copy_from_cart(cartPtr, len);
  }
  return out;
}

// copy from a host pointer to cart, return cart pointer
EM_JS(int, copy_to_cart, (void* hostPtr, unsigned int size), {
  const cartPtr = Module.cart.exports.malloc(size);
  const cartBytes = Module.HEAPU8.slice(hostPtr, hostPtr + size);
  const mem = new Uint8Array(Module.cart.exports.memory.buffer);
  mem.set(cartBytes, cartPtr);
  return cartPtr;
});

/// API

// send a string to host
void EMSCRIPTEN_KEEPALIVE test_string_in(unsigned int sPtr){
  char* str = copy_from_cart_string(sPtr);
  printf("host: test_string_in - %s\n", str);
}

// return a string from host
unsigned int EMSCRIPTEN_KEEPALIVE test_string_out() {
  char* ret = "hello!";
  return copy_to_cart(ret, strlen(ret) + 1);
}

// send some bytes to host
void EMSCRIPTEN_KEEPALIVE test_bytes_in(unsigned int bytesPtr, unsigned int bytesLen) {
  unsigned char* bytes = copy_from_cart(bytesPtr, bytesLen);
  printf("host: test_bytes_in (%u) - %u %u %u %u\n", bytesLen, bytes[0], bytes[1], bytes[2], bytes[3]);
}

// return some bytes from host
unsigned int EMSCRIPTEN_KEEPALIVE test_bytes_out(unsigned int outLenPtr) {
  unsigned int outLen = 4;

  unsigned char bytes[] = {0,1,2,3};
  return copy_to_cart(bytes, outLen);
}

// send struct to host
void EMSCRIPTEN_KEEPALIVE test_struct_in(unsigned int pointPntr) {
  TestPoint* point = copy_from_cart(pointPntr, sizeof(TestPoint));
  printf("host: test_struct_in - %ux%u\n", point->x, point->y);
}

// return struct from host
unsigned int EMSCRIPTEN_KEEPALIVE test_struct_out(){
  TestPoint point = {.x=200, .y=100};
  return copy_to_cart(&point, sizeof(point));
}

///
