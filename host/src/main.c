// basic stuff that gets used all over
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define sleep(x) usleep((x)*1000)
#endif

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"

// filesystem utils (used by host)
#include "fs.h"

// host implementations for emscripten/wamr
#include "host.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CART_FILE>\n", argv[0]);
    return 1;
  }

  if (!fs_init(argv[1])) {
    fprintf(stderr, "Could not initialize filesystem with %s\n", argv[1]);
    return 1;
  }

  if (!wasm_host_load()) {
    fprintf(stderr, "Could not start cart-host with %s\n", argv[1]);
    return 1;
  }

  #ifdef EMSCRIPTEN
    emscripten_set_main_loop(wasm_host_update, 60, false);
  #else
    while(1) {
      wasm_host_update();
      sleep(0.016f); // ~60fps
    }
  #endif

  wasm_host_unload();
  return 0;
}
