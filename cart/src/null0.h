#include <stdlib.h>

#define NULL0_EXPORT(n) __attribute__((export_name(n)))
#define NULL0_IMPORT(n) __attribute__((import_module("null0"), import_name(n)))

// mem-management

NULL0_EXPORT("malloc")
void* _null0_malloc(size_t size) {
  return malloc(size);
}

NULL0_EXPORT("free")
void _null0_free(void* ptr) {
  free(ptr);
}

//// Cart callbacks

NULL0_EXPORT("update")
void update(int time);
NULL0_EXPORT("unload")
void unload();
NULL0_EXPORT("buttonUp")
void buttonUp(unsigned int button);
NULL0_EXPORT("buttonDown")
void buttonDown(unsigned int button);
NULL0_EXPORT("keyUp")
void keyUp(unsigned int key);
NULL0_EXPORT("keyDown")
void keyDown(unsigned int key);
NULL0_EXPORT("load")
void load();

// these are some testers for arg/ret passing

typedef struct {
  unsigned int x;
  unsigned int y;
} TestPoint;

// send a string to host
NULL0_IMPORT("test_string_in")
void test_string_in(char* str);

// return a string from host
NULL0_IMPORT("test_string_out")
char* test_string_out();

// send some bytes to host
NULL0_IMPORT("test_bytes_in")
void test_bytes_in(unsigned char* bytes, unsigned int bytesLen);

// return some bytes from host
NULL0_IMPORT("test_bytes_out")
unsigned char* test_bytes_out(unsigned int* outLen);

// send struct to host
NULL0_IMPORT("test_struct_in")
void test_struct_in(TestPoint* point);

// return struct from host
NULL0_IMPORT("test_struct_out")
TestPoint* test_struct_out();
