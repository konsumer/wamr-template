// this is the emscripten-only host callbacks

#pragma once

// called on web-side JS to load cart into MOdule and expose host-functions to it
EM_ASYNC_JS(bool, wasm_host_load_wasm, (unsigned char* wasmBytesPtr, uint32_t wasmBytesLen), {
  const wasmBytes = Module.HEAPU8.slice(wasmBytesPtr, wasmBytesPtr+wasmBytesLen);
  const d = new TextDecoder();
  const importObject = {
    null0: {},
    wasi_snapshot_preview1: Module.wasi1_instance
  };

  // shared util to get a strlen, max 1024*1024
  Module.cart_strlen = (s) => new Uint8Array(Module.cart.memory.buffer.slice(s, s+(1024*1024))).findIndex((b) => b == 0);

  // bind any host exports that start with host_
  for (const k of Object.keys(Module)) {
    if (k.startsWith('_host_')) {
      importObject.null0[k.replace(/^_host_/, "")] = Module[k];
    }
  }

  const { instance: { exports } } = await WebAssembly.instantiate(wasmBytes, importObject);
  Module.cart = exports;
  Module.wasi1_instance.start(Module.cart);
  if (Module.cart.load) {
    Module.cart.load();
  }

  return true;
});

// called on each frame
EM_JS(void, wasm_host_update, (), {
  if ( Module?.cart?.update){
    Module.cart.update(BigInt(Date.now()));
  }
})

// called when cart is unloaded
void wasm_host_unload() {}
