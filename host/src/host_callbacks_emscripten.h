// this is the emscripten-only host callbacks

#pragma once

// called when cart is loaded
bool wasm_host_load() {
  uint32_t wasmBytesLen = 0;
  unsigned char* wasmBytes = fs_load_file("main.wasm", &wasmBytesLen);
  if (wasmBytesLen == 0) {
    return false;
  }
  return true;
}

// called when cart is unloaded
void wasm_host_unload() {}

// called on each frame
void wasm_host_update() {}
