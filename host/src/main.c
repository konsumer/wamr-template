// host implementations for emscripten/wamr

#include "host.h"

// set this to false to stop
bool keepRunning = true;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CART_FILE>\n", argv[0]);
    return 1;
  }

  if (!fs_init(argv[1])) {
    fprintf(stderr, "Could not initialize filesystem with %s\n", argv[1]);
    return 1;
  }

  DetectFileType d = fs_detect_type_real(argv[1]);

  if (d == FILE_TYPE_UNKNOWN) {
    fprintf(stderr, "Invalid cart %s\n", argv[1]);
    return 1;
  }

  char* cartFilename = d == FILE_TYPE_WASM ? basename(argv[1]) : "main.wasm";
  if (!wasm_host_load(cartFilename)) {
    fprintf(stderr, "Could not start cart-host with %s\n", argv[1]);
    return 1;
  }

  // printf("loaded: %s\n", cartFilename);

  #ifdef EMSCRIPTEN
    emscripten_set_main_loop(wasm_host_update, 60, false);
  #else
    while(keepRunning) {
      wasm_host_update();
      sleep(0.016f); // ~60fps
    }
  #endif

  wasm_host_unload();
  return 0;
}
