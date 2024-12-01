This contains a native and web WASI host. The idea is that you are making something like [null0](https://giuthub.com/natnullgames/null0) where you have a host written in C, and "carts" written in whatever the user wants.

I wanted to be able to spin up host ideas for [null0](https://giuthub.com/natnullgames/null0) quickjly, but you can use it for whatever you like. It uses WASI `preview1` so it will work with any wasm-language that support that.

## features

- Minimal web-host, that runs carts & incldues WASI and any functions you exported from host
- Minimal wamr native host that uses the same WASI to access the shared filesystem
- Build hosts and cart easily (with cmake)
- Test API to show how to pass more advanced values back & forth
- shared filesystem that uses a zip-file
- (planned) Codegen to use a YAML file to describe your API (and generate the host & C cart-header)

## building

```
# native host & cart
npm run build

# emscripten host
npm run build:web

# run a server for web-host
npm start
```

## Test API

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

It's already been setup in cart & hosts, so you can get an idea of how you will need to expose your own API.

## notes

These might help with wamr:

- [embed_wamr](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/embed_wamr.md)
- [export_native_api](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)
