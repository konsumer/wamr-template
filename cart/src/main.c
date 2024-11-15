#include <stdio.h>
#include "null0.h"

void update(int time) {}

void load() {
  printf("cart: Hello from cart load.\n");

  test_string_in("hi");
  printf("cart: test_string_out - %s\n", test_string_out());

  unsigned char i[4] = {1, 2, 3, 4};
  test_bytes_in(i, 4);

  unsigned int len = 0;
  unsigned char* bytes = test_bytes_out(&len);
  printf("cart: test_bytes_out (%u) - %u %u %u %u\n", len, bytes[0], bytes[1], bytes[2], bytes[3]);

  TestPoint p = {100, 200};
  test_struct_in(&p);

  TestPoint* p2 = test_struct_out();
  printf("cart: test_struct_out - %ux%u\n", p2->x, p2->y);
}

int main() {
  printf("cart: Hello from cart main.\n");
  return 0;
}
