#include "host.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CART_FILE>\n", argv[0]);
    return 1;
  }
  
  wasm_host_load(argv[1]);

  if (cart_update != NULL) {
    while (1) {
      wasm_host_update();
    }
  }

  wasm_host_unload();
  return 0;
}
