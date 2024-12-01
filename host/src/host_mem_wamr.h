// this is the wamr-only host memory helpers

wasm_module_inst_t module_inst;

// copy a pointer from cart to host
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  void* out = malloc(size);
  void* cartHostPtr = wasm_runtime_addr_app_to_native(module_inst, (uint64_t)cartPtr);
  memcpy(out, cartHostPtr, size);
  return out;
}

// copy a pointer from host to cart
unsigned int copy_to_cart(void* hostPtr, unsigned int size) {
  return (unsigned int) wasm_runtime_module_dup_data(module_inst, (const char*)hostPtr, (uint64_t) size);
}
