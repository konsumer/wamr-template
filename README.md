This contains a native and web WASI host.

## notes

- [embed_wamr](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/embed_wamr.md)
- [export_native_api](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)

## building

```
# native host & cart
cmake -B build
cmake --build build

# emscripten host
emcmake cmake -B wbuild
cmake --build wbuild
```

## test api

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

Web-host uses automatic exports (anything you expose will be exposed to cart.)
