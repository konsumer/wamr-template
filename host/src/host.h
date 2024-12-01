// implement any shared host types & functiosn here

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// get current unix-time in ms
static int null0_millis() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return ((unsigned int)now.tv_sec) * 1000 + ((unsigned int)now.tv_nsec) / 1000000;
}

// implement these memory-helpers for each host
unsigned int copy_to_cart(void* hostPtr, unsigned int size);
void* copy_from_cart(unsigned int cartPtr, unsigned int size);
int cart_strlen(unsigned int cartPtr);

// implement these callbacks for each host
int wasm_host_load(char* filename);
void wasm_host_unload();
void wasm_host_update();

// copy a pointer to a string from cart to host
char* copy_from_cart_string(unsigned int cartPtr) {
  int len = cart_strlen(cartPtr);
  char* out = NULL;
  if (len) {
    out = copy_from_cart(cartPtr, len);
  }
  return out;
}

#ifdef EMSCRIPTEN
  #include "emscripten.h"
  #include "host_mem_emscripten.h"
  #define HOST_PARAMS(...) __VA_ARGS__
  #define HOST_FUNCTION(ret_type, name, params, body) EMSCRIPTEN_KEEPALIVE ret_type host_#name(HOST_PARAMS params) body
#else
  #include "wasm_c_api.h"
  #include "wasm_export.h"
  #include "host_mem_wamr.h"
  #define HOST_PARAMS(...) __VA_ARGS__
  #define HOST_FUNCTION(ret_type, name, params, body) ret_type host_#name(wasm_exec_env_t exec_env, HOST_PARAMS params) body
#endif

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
