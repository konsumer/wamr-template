// this is the emscripten-only host memory helpers

#pragma once

// copy a host-pointer to cart, return cart-pointer
unsigned int copy_to_cart(void* hostPtr, unsigned int size) {
  return 0;
}

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size) {
  return NULL;
}

// get the strlen of a cart-pointer
int cart_strlen(unsigned int cartPtr) {
  return 0;
}