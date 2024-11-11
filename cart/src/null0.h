#define NULL0_EXPORT(n) __attribute__((export_name(n)))
#define NULL0_IMPORT(n) __attribute__((import_module("null0"), import_name(n)))

//// Cart callbacks

NULL0_EXPORT("update") void update(int time);
NULL0_EXPORT("unload") void unload();
NULL0_EXPORT("buttonUp") void buttonUp(unsigned int button);
NULL0_EXPORT("buttonDown") void buttonDown(unsigned int button);
NULL0_EXPORT("keyUp") void keyUp(unsigned int key);
NULL0_EXPORT("keyDown") void keyDown(unsigned int key);
NULL0_EXPORT("load") void load();
