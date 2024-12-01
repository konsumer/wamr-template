#include <stdio.h>
#include "host.h"
#include "fs.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CART_FILE>\n", argv[0]);
    return 1;
  }

  if (!fs_init(argv[1])) {
    fprintf(stderr, "Could not initialize filesystem with %s\n", argv[1]);
    return 1;
  }

  if (!wasm_host_load() {
    fprintf(stderr, "Could notstart host with %s\n", argv[1]);
    return 1;
  }

  #ifdef EMSCRIPTEN
    emscripten_set_main_loop(wasm_host_update, 60, false);
  #else
    while(1) {
      wasm_host_update();
    }
  #endif

  wasm_host_unload();
  return 0;
}
