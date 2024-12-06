// shared definition for different hosts

#pragma once

// basic stuff that gets used all over
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define FS_IMPLEMENTATION
#include "fs.h"

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"

// simplified type-names
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

// cross-platform sleep
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define sleep(x) usleep((x)*1000)
#endif

// HOST

// implement these memory-helpers for each host (in host_(emscripten|wamr).c)

// copy a host-pointer to cart when you already have a cart-pointer
void copy_to_cart_with_pointer(u32 cartPtr, void* hostPtr, u32 size);

// copy a cart-pointer to host when you already have a host-pointer
void copy_from_cart_with_pointer(void* hostPtr, u32 cartPtr, u32 size);

// allocate some memory in cart
u32 cart_malloc(u32 size);

// free some memory in cart
void cart_free(u32 ptr);

// get the strlen of a cart-pointer
u32 cart_strlen(u32 cartPtr);

// implement these callbacks for each host

// called on native wamr/web host to load actual cart bytes expose host-functions to it
bool wasm_host_load_wasm (u8* wasmBytesPtr, u32 wasmBytesLen);

// called when cart is unloaded
void wasm_host_unload_wasm();

// called on each frame
void wasm_host_update();



// these are shared and derived from the other functions (in host.c)

// copy a host-pointer to cart, return cart-pointer
u32 copy_to_cart(void* hostPtr, u32 size);

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(u32 cartPtr, u32 size);

// copy a cart-pointer to a host-string
char* copy_from_cart_string(u32 cartPtr);

// copy a host-string to a cart-pointer
u32 copy_to_cart_string(char* hostString);

// load file & calls wasm_host_load_wasm
bool wasm_host_load(char* wasmFilename);

// called when cart is unloaded
void wasm_host_unload();

// macro for single definitions of host functions
#ifdef EMSCRIPTEN
  #include "emscripten.h"
  #define HOST_FUNCTION(ret_type, name, params, ...) EMSCRIPTEN_KEEPALIVE ret_type host_##name params { __VA_ARGS__ }
  #include "host_emscripten.h"
#else
  #include "wasm_c_api.h"
  #include "wasm_export.h"
  #define EXPAND_PARAMS(...) , ##__VA_ARGS__
  #define HOST_FUNCTION(ret_type, name, params, ...) \
    ret_type host_##name(wasm_exec_env_t exec_env EXPAND_PARAMS params) { __VA_ARGS__ }; \
    static void __attribute__((constructor)) _register_##name() { \
      cvector_push_back(null0_native_symbols, ((NativeSymbol){ #name, host_##name, NULL })); \
    }
    #include "host_wamr.h"
#endif

// copy a host-pointer to cart, return cart-pointer
u32 copy_to_cart(void* hostPtr, u32 size) {
  u32 cartPtr = cart_malloc(size);
  copy_to_cart_with_pointer(cartPtr, hostPtr, size);
  return cartPtr;
}

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(u32 cartPtr, u32 size) {
  void* hostPtr = malloc(size);
  copy_from_cart_with_pointer(hostPtr, cartPtr, size);
  return hostPtr;
}

// copy a cart-pointer to a host-string
char* copy_from_cart_string(u32 cartPtr) {
  u32 size = cart_strlen(cartPtr);
  return copy_from_cart(cartPtr, size + 1);
}

// copy a host-string to a cart-pointer
u32 copy_to_cart_string(char* hostPtr) {
  u32 size = strlen(hostPtr);
  return copy_to_cart(hostPtr, size + 1);
}

// load file & calls wasm_host_load_wasm
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

// called when cart is unloaded
void wasm_host_unload() {
  wasm_host_unload_wasm();
  // TODO: clean up things
}
