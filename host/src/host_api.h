#pragma once

typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// helpful utils

HOST_FUNCTION(void, trace, (unsigned int sPtr), {
  printf("%s\n", copy_from_cart_string(sPtr));
})

HOST_FUNCTION(void, abort, (unsigned int mPtr, unsigned int fPtr, unsigned int line, unsigned int column), {
  fprintf(stderr, "%s in %s:%u:%u\n", copy_from_cart_string(mPtr), copy_from_cart_string(fPtr), line, column);
  keepRunning = false;
})

// test API

// send a string to host
HOST_FUNCTION(void, test_string_in, (unsigned int sPtr), {
  char* str = copy_from_cart_string(sPtr);
  printf("host: test_string_in - %s\n", str);
})

// return a string from host
HOST_FUNCTION(unsigned int, test_string_out, (), {
  char* s = "hello from host!";
  unsigned int retPtr = copy_to_cart_string(s);
  return retPtr;
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
  copy_to_cart_with_pointer(outLenPtr, &outLen, sizeof(outLen));
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
