// implement any shared host types & functiosn here

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// get current unix-time in ms
static int null0_millis() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return ((unsigned int)now.tv_sec) * 1000 + ((unsigned int)now.tv_nsec) / 1000000;
}

// HOST: implement these memory-helpers for each host

// copy a host-pointer to cart, return cart-pointer
unsigned int copy_to_cart(void* hostPtr, unsigned int size);

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size);

// get the strlen of a cart-pointer
int cart_strlen(unsigned int cartPtr);

// copy a cart-pointer to a host-string
char* copy_from_cart_string(unsigned int cartPtr) {
  int len = cart_strlen(cartPtr);
  char* out = NULL;
  if (len) {
    out = copy_from_cart(cartPtr, len);
  }
  return out;
}

// copy a host-string to a cart-pointer
unsigned int copy_to_cart_string(char* hostString) {

}


// HOST: implement these callbacks for each host

// called when cart is loaded
bool wasm_host_load();

// called when cart is unloaded
void wasm_host_unload();

// called on each frame
void wasm_host_update();


#ifdef EMSCRIPTEN
  #include "emscripten.h"
  #include "host_mem_emscripten.h"
  #define HOST_FUNCTION(ret_type, name, params, ...) \
    EMSCRIPTEN_KEEPALIVE ret_type host_##name params { __VA_ARGS__ }
#else
  #include "wasm_c_api.h"
  #include "wasm_export.h"
  #include "host_mem_wamr.h"
  #define EXPAND_PARAMS(...) , ##__VA_ARGS__
  #define HOST_FUNCTION(ret_type, name, params, ...) \
    ret_type host_##name(wasm_exec_env_t exec_env EXPAND_PARAMS params) { __VA_ARGS__ }
#endif

// test API

typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// send a string to host
HOST_FUNCTION(void, test_string_in, (unsigned int sPtr), {
  char* str = copy_from_cart_string(sPtr);
  printf("host: test_string_in - %s\n", str);
})

// return a string from host
HOST_FUNCTION(unsigned int, test_string_out, (), {
  char* ret = "hello!";
  return copy_to_cart(ret, strlen(ret) + 1);
})

// send some bytes to host
HOST_FUNCTION(void, test_bytes_in, (unsigned int bytesPtr, unsigned int bytesLen), {
  unsigned char* bytes = copy_from_cart(bytesPtr, bytesLen);
  printf("host: test_bytes_in (%u) - %u %u %u %u\n", bytesLen, bytes[0], bytes[1], bytes[2], bytes[3]);
})

// return some bytes from host
HOST_FUNCTION(unsigned int, test_bytes_out, (unsigned int outLenPtr), {
  unsigned int outLen = 4;
  unsigned char bytes[] = {0,1,2,3};
  return copy_to_cart(bytes, outLen);
})

// send struct to host
HOST_FUNCTION(void, test_struct_in, (unsigned int pointPntr), {
  TestPoint* point = copy_from_cart(pointPntr, sizeof(TestPoint));
  printf("host: test_struct_in - %ux%u\n", point->x, point->y);
})

// return struct from host
HOST_FUNCTION(unsigned int, test_struct_out, (), {
  TestPoint point = {.x=200, .y=100};
  return copy_to_cart(&point, sizeof(point));
})

#ifdef EMSCRIPTEN
  #include "host_callbacks_emscripten.h"
#else
  #include "host_callbacks_wamr.h"
#endif
