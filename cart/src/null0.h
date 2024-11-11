#include <stdlib.h>

#define NULL0_EXPORT(n) __attribute__((export_name(n)))
#define NULL0_IMPORT(n) __attribute__((import_module("null0"), import_name(n)))

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
} Point;

NULL0_IMPORT("test_string_in")  // ($)
void test_string_in(char* i);

NULL0_IMPORT("test_string_out")  // (i)
void __test_string_out(char* o);
char* test_string_out() {
  // TODO: host should malloc using wasm_runtime_module_dup_data
  char* o = malloc(1024);
  __test_string_out(o);
  return o;
}

NULL0_IMPORT("test_bytes_in")  // (*~)
void test_bytes_in(unsigned char* i, unsigned int iLen);

NULL0_IMPORT("test_bytes_out")  // (**)
void __test_bytes_out(unsigned char* out, unsigned int* olen);
unsigned char* test_bytes_out(unsigned int* len) {
  unsigned char* out = malloc(1024);
  __test_bytes_out(out, len);
  return out;
}

NULL0_IMPORT("test_struct_in")  // (*~)
void __test_struct_in(Point* i, unsigned int len);
void test_struct_in(Point* i) {
  __test_struct_in(i, sizeof(Point));
}

NULL0_IMPORT("test_struct_out")  // (i)
void __test_struct_out(Point* i);
Point* test_struct_out() {
  Point* out = {};
  __test_struct_out(out);
  return out;
}
