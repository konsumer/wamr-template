#include <stdio.h>
#include "cart.h"

typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// send a string to host
HOST_FUNCTION(void, test_string_in, (char* str))

// return a string from host
HOST_FUNCTION(char*, test_string_out, ())

// send some bytes to host
HOST_FUNCTION(void, test_bytes_in, (unsigned char* bytes, unsigned int bytesLen))

// return some bytes from host
HOST_FUNCTION(unsigned char*, test_bytes_out, (unsigned int* outLenPtr))

// send struct to host
HOST_FUNCTION(void, test_struct_in, (TestPoint* point))

// return struct from host
HOST_FUNCTION(TestPoint*, test_struct_out, ())


// tests built-in type-testing functions
void run_tests() {
  test_string_in("cart (runtests): hi");
  printf("cart (runtests): test_string_out - %s\n", test_string_out());

  unsigned char i[4] = {1, 2, 3, 4};
  test_bytes_in(i, 4);

  unsigned int len = 0;
  unsigned char* bytes = test_bytes_out(&len);
  printf("cart (runtests): test_bytes_out (%u) - %u %u %u %u\n", len, bytes[0], bytes[1], bytes[2], bytes[3]);

  TestPoint p = {100, 200};
  test_struct_in(&p);

  TestPoint* p2 = test_struct_out();
  printf("cart (runtests): test_struct_out - %ux%u\n", p2->x, p2->y);
}


void update(uint64_t timeMS) {}

void load(){
  printf("cart (runtests): Hello from cart load.\n");
}

int main() {
  printf("cart (runtests): Hello from cart main.\n");
  run_tests();
  return 0;
}
