// Host interface exposed to WAMR and Emscripten

#pragma once

#include "hexdump.h"

// Used to test passing structs over wasm-barrier
typedef struct {
  u32 x;
  u32 y;
} TestPoint;

// UTILS API

// print a string
HOST_FUNCTION(void, trace, (u32 strPtr), {
  char* str = copy_from_cart_string(strPtr);
  printf("%s\n", str);
  free(str);
})

// a fatal exit
HOST_FUNCTION(void, abort, (u32 messagePtr, u32 filenamePtr, u32 line, u32 column), {
  char* message = copy_from_cart_string(messagePtr);
  char* filename = copy_from_cart_string(filenamePtr);
  fprintf(stderr, "%s: %s:%u:%u\n", message, filename, line, column);
  free(message);
  free(filename);
})

// TEST API

// send a string to host
HOST_FUNCTION(void, test_string_in, (u32 strPtr), {
  char* str = copy_from_cart_string(strPtr);
  printf("host test_string_in: got string from cart: %s\n", str);
  free(str);
})

// return a string from host
HOST_FUNCTION(u32, test_string_out, (), {
  char* s = "hello from host";
  return copy_to_cart_string(s);
})

// send some bytes to host
HOST_FUNCTION(void, test_bytes_in, (u32 bytesPtr, u32 bytesLen), {
  u8* bytes = copy_from_cart(bytesPtr, bytesLen);
  printf("host test_bytes_in:\n");
  hexdump(bytes, bytesLen);
  free(bytes);
})

// return some bytes from host
HOST_FUNCTION(u32, test_bytes_out, (u32 outLenPtr), {
  u32 outLen = 4;
  u8 returnVal[4] = {1,2,3,4};
  copy_to_cart_with_pointer(outLenPtr, &outLen, sizeof(outLen));
  return copy_to_cart(returnVal, outLen);
})

// send struct to host
HOST_FUNCTION(void, test_struct_in, (u32 pointPtr), {
  TestPoint* point = copy_from_cart(pointPtr, sizeof(TestPoint));
  printf("host test_struct_in: (%u, %u)\n", point->x, point->y);
  free(point);
})

// return struct from host
HOST_FUNCTION(u32, test_struct_out, (), {
  TestPoint result = { .x=1111, .y=2222 };
  return copy_to_cart(&result, sizeof(result));
})
