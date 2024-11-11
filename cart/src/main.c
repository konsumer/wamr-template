#include <stdio.h>
#include <string.h>
#include "null0.h"

void update(int time) {}

void load() {
  printf("cart: Hello from cart load.\n");

  test_string_in("hi");
  printf("cart: from host: %s\n", test_string_out());

  unsigned char i[4] = {1, 2, 3, 4};
  test_bytes_in(i, 4);

  unsigned int len = 0;
  unsigned char* bytes = test_bytes_out(&len);
  printf("cart: test_bytes_out (%u): %u %u %u %u\n", len, bytes[0], bytes[1], bytes[2], bytes[3]);

  Point p = {100, 200};
  test_struct_in(&p);

  Point* p2 = test_struct_out();
  printf("cart: host sent point %ux%u\n", p2->x, p2->y);
}

int main() {
  printf("cart: Hello from cart main.\n");
  return 0;
}
