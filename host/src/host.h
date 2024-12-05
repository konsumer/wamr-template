// implement any shared host types & functiosn here

#pragma once

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef float f32;
typedef double f64;

// HOST: implement these memory-helpers for each host

// copy a host-pointer to cart, return cart-pointer
unsigned int copy_to_cart(void* hostPtr, unsigned int size);

// copy a host-pointer to cart whenb you already have a cart-pointer
void copy_to_cart_with_pointer(unsigned int outPtr, void* hostPtr, unsigned int size);

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size);

// get the strlen of a cart-pointer
int cart_strlen(unsigned int cartPtr);


// HOST: implement these callbacks for each host

// called on native wamr/web host to load actual cart bytes expose host-functions to it
bool wasm_host_load_wasm (unsigned char* wasmBytesPtr, uint32_t wasmBytesLen);

// this is defined at end of this file, it just loads file & calls wasm_host_load_wasm
bool wasm_host_load(char* wasmFilename);

// called when cart is unloaded
void wasm_host_unload();

// called on each frame
void wasm_host_update();

#ifdef EMSCRIPTEN
  #include "host_emscripten_header.h"
  #define HOST_FUNCTION(ret_type, name, params, ...) EMSCRIPTEN_KEEPALIVE ret_type host_##name params { __VA_ARGS__ }
#else
  #include "host_wamr_header.h"
  #define EXPAND_PARAMS(...) , ##__VA_ARGS__
  // #define HOST_FUNCTION(ret_type, name, params, ...) ret_type host_##name(wasm_exec_env_t exec_env EXPAND_PARAMS params) { __VA_ARGS__ };
  cvector_vector_type(NativeSymbol) null0_native_symbols = NULL;

  #define HOST_FUNCTION(ret_type, name, params, ...) \
    ret_type host_##name(wasm_exec_env_t exec_env EXPAND_PARAMS params) { __VA_ARGS__ }; \
    static void __attribute__((constructor)) _register_##name() { \
      cvector_push_back(null0_native_symbols, ((NativeSymbol){ #name, host_##name, NULL })); \
    }
#endif

// these are derived from copy_to_cart/cart_strlen/etc

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void* out = malloc(size);
  copy_from_cart_with_pointer(out, cartPtr, size);
  return out;
}

// copy a cart-pointer to a host-string
char* copy_from_cart_string(unsigned int cartPtr) {
  int len = cart_strlen(cartPtr);
  char* out = (char*)malloc(len+1);
  if (len) {
    out = (char*)copy_from_cart(cartPtr, len + 1);
  }
  return out;
}

// copy a host-string to a cart-pointer
unsigned int copy_to_cart_string(char* hostString) {
  return copy_to_cart(hostString, strlen(hostString) + 1);
}

#include "host_api.h"

#ifdef EMSCRIPTEN
  #include "host_emscripten_footer.h"
#else
  #include "host_wamr_footer.h"
#endif

bool wasm_host_load(char* wasmFilename) {
  if (fs_detect_type(wasmFilename) != FILE_TYPE_WASM) {
    return false;
  }
  uint32_t wasmBytesLen = 0;
  unsigned char* wasmBytes = fs_load_file(wasmFilename, &wasmBytesLen);
  if (wasmBytesLen == 0) {
    return false;
  }
  return wasm_host_load_wasm(wasmBytes, wasmBytesLen);
}
