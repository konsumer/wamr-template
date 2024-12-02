#include <stdlib.h>
#include <stdint.h>

// import function from host
#define HOST_FUNCTION(return_type, name, params) __attribute__((import_module("null0"), import_name(#name))) return_type name params;

// export function to host
#define CART_FUNCTION(n) __attribute__((export_name(n)))

// mem-management

CART_FUNCTION("malloc")
void* _wasm_host_malloc(size_t size) {
  return malloc(size);
}

CART_FUNCTION("free")
void _wasm_host_free(void* ptr) {
  free(ptr);
}

// callbacks
CART_FUNCTION("load")
void load();

CART_FUNCTION("update")
void update(uint64_t timeMS);

CART_FUNCTION("unload")
void unload();
