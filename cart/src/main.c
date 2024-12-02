#include <stdio.h>
#include "cart.h"
#include "typetests.h"

void update(uint64_t timeMS) {}

void load(){
  printf("cart (c): Hello from cart load.\n");
}

int main() {
  printf("cart (c): Hello from cart main.\n");
  run_tests();
  return 0;
}
