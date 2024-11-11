#include "host.h"

int main(int argc, char *argv[]) {
  wasm_host_load("build/cart/main.wasm");

  if (cart_update != NULL) {
    while (1) {
      wasm_host_update();
    }
  }

  wasm_host_unload();
  return 0;
}
