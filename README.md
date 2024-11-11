This contains a native and web WASI host.

## notes

- [embed_wamr](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/embed_wamr.md)
- [export_native_api](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)
- [multi_module](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/multi_module.md)

## building

```
cmake -B build
cmake --build build
```

## test api

There is a tester API that looks like this, from the user's perspective:

```c
typedef struct {
  unsigned int x;
  unsigned int y;
} Point;

// send a string to host
void test_string_in(char *i);

// return a string from host
char *test_string_out();

// send some bytes to host
void test_bytes_in(unsigned char *i, unsigned int iLen);

// return some bytes from host
unsigned char *test_bytes_out(unsigned int *len);

// send struct to host
void test_struct_in(Point *i);

// return struct from host
Point *test_struct_out();
```
