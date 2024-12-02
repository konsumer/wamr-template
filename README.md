This contains a native and web WASI host. The idea is that you are making something like [null0](https://giuthub.com/natnullgames/null0) where you have a host written in C, and "carts" written in whatever the user wants.

I wanted to be able to spin up host ideas for [null0](https://giuthub.com/natnullgames/null0) quickly, but you can use it for whatever you like. It uses WASI `preview1` so it will work with any wasm-language that support that.

## features

- Minimal web-host, that runs carts & incldues WASI and any functions you exported from host
- Minimal wamr native host that uses the same WASI to access the shared filesystem
- Build hosts and cart easily (with cmake)
- Test API to show how to pass more advanced values back & forth
- shared filesystem that uses a zip-file
- (planned) Codegen to use a YAML or wit file to describe API (and generate the host & C cart-header)

## building

```
# get build-tools and stuff
npm i

# native/emscripten host host & carts
npm run build

# just carts
npm run build:carts

# just emscripten host
npm run build:web

# just native host
npm run build:native

# run a live-reloading server for web (carts and host)
npm start
```

## host.h

There is a tester API that looks like this:

```c
typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// send a string to host
void test_string_in(char* str);

// return a string from host
char* test_string_out();

// send some bytes to host
void test_bytes_in(unsigned char* bytes, unsigned int bytesLen);

// return some bytes from host
unsigned char* test_bytes_out(unsigned int* outLen);

// send struct to host
void test_struct_in(TestPoint* point);

// return struct from host
TestPoint* test_struct_out();
```

## helpers

In order to implement these, I also implemented some helpers you can use in your own thing:

```c
// copy a host-pointer to cart, return cart-pointer
unsigned int copy_to_cart(void* hostPtr, unsigned int size);

// copy a cart-pointer to host, return host-pointer
void* copy_from_cart(unsigned int cartPtr, unsigned int size);

// get the strlen of a cart-pointer
int cart_strlen(unsigned int cartPtr);

// copy a cart-pointer to a host-string
char* copy_from_cart_string(unsigned int cartPtr);

// copy a host-string to a cart-pointer
unsigned int copy_to_cart_string(char* hostString);
```

There is also a macro for defining a host-function:

```c
HOST_FUNCTION(void, test_string_in, (unsigned int sPtr), {
  char* str = copy_from_cart_string(sPtr);
  printf("host: test_string_in - %s\n", str);
})
```

This will expose a function that looks like this:

```c
void host_test_string_in(unsigned int sPtr);
```

cart.h also has some macros:

```c
// to export
CART_FUNCTION("name_of_function_export")
void* _your_function() {
}

// to import
HOST_FUNCTION(void, test_string_in, (unsigned int sPtr))
```

## notes

These might help with wamr:

- [embed_wamr](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/embed_wamr.md)
- [export_native_api](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)
