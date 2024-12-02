// this implements shared filesystem functions

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "physfs.h"

static char** null0_file_list_array;

// these are the supported filetypes we can detect
// see fs_parse_magic_bytes()
typedef enum {
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_ZIP,
    FILE_TYPE_WASM,
    FILE_TYPE_PNG,
    FILE_TYPE_JPEG,
    FILE_TYPE_WAV,
    FILE_TYPE_OGG,
    FILE_TYPE_MP3,
    FILE_TYPE_DIR
} DetectFileType;

// call this to initialize filesystem
bool fs_init(char* cartName);

// called to unload filesystem
void fs_unload();

// load a file from native filesystem
unsigned char* fs_load_file_real(char *filename, unsigned int *bytesRead);

// load a file from physfs filesystem
unsigned char* fs_load_file(char* filename, uint32_t* bytesRead);

// save a file to native filesystem
bool fs_save_file_real(char* filename, unsigned char* data, uint32_t byteSize);

// save a file to physfs filesystem
bool fs_save_file(char* filename, unsigned char* data, uint32_t byteSize);

// just detect filetype from first 4 bytes
DetectFileType fs_parse_magic_bytes(uint32_t magic_number);

// detect file-type from native filesystem file
DetectFileType fs_detect_type_real(char* filename);

// detect file-type from physfs filesystem file
DetectFileType fs_detect_type(char* filename);

// get the short-name of cart, using filename
char* fs_get_cart_name(char* filename);

// Get info about a file from native filesystem
PHYSFS_Stat fs_file_info(char* filename);


// call this to initialize filesystem
bool fs_init(char* cartFilename) {
  // logic:
  //   mount root & write-dir (per-cartname)
  //   if cartName=dir: mount dir
  //   if cartName=zip: mount zip
  //   if cartName=wasm: mount dir of wasm
  //   else: return false

  char* cartName = fs_get_cart_name(cartFilename);

  if (cartName == NULL) {
    return false;
  }

  char pathname[134];
  snprintf(pathname, 134, "null0-%s", cartName);
  const char* null0_writable_dir = PHYSFS_getPrefDir("null0", (const char *)pathname);

  if (null0_writable_dir == NULL) {
    return false;
  }

  if (!PHYSFS_init("/")) {
    return false;
  }

  DetectFileType cartType = fs_detect_type_real(cartFilename);
  switch(cartType) {
    case FILE_TYPE_DIR:
    case FILE_TYPE_ZIP: {
      if (!PHYSFS_mount(cartFilename, NULL, 1)) {
        PHYSFS_deinit();
        return false;
      }
    }
    case FILE_TYPE_WASM: {}
    default: return false;
  }

  if (!PHYSFS_mount(null0_writable_dir, NULL, 1)) {
    PHYSFS_deinit();
    return false;
  }

  if (!PHYSFS_setWriteDir(null0_writable_dir)) {
    PHYSFS_deinit();
    return false;
  }

  return true;
}

// called to unload filesystem
void fs_unload() {
  PHYSFS_deinit();
  PHYSFS_freeList(null0_file_list_array);
}

// load a file from native filesystem
unsigned char* fs_load_file_real(char *filename, unsigned int *bytesRead) {
  // TODO
  return NULL;
}

// load a file from physfs filesystem
unsigned char* fs_load_file(char* filename, uint32_t* bytesRead) {
  PHYSFS_File* f = PHYSFS_openRead(filename);
  PHYSFS_Stat i = fs_file_info(filename);

  unsigned char* b = (unsigned char*)malloc(i.filesize);
  PHYSFS_sint64 br = PHYSFS_readBytes(f, b, i.filesize);
  *bytesRead = br;
  PHYSFS_close(f);
  return b;
}

// save a file to native filesystem
bool fs_save_file_real(char* filename, unsigned char* data, uint32_t byteSize) {
  // TODO
  return false;
}

// save a file to physfs filesystem
bool fs_save_file(char* filename, unsigned char* data, uint32_t byteSize) {
  PHYSFS_File* f = PHYSFS_openWrite(filename);
  PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(f, data, byteSize);
  PHYSFS_close(f);
  if (byteSize != bytesWritten) {
    return false;
  }
  return true;
}

// just detect filetype from first 4 bytes
DetectFileType fs_parse_magic_bytes(uint32_t magic_number) {
  switch (magic_number) {
    case 0x4034b50:  // ZIP
        return FILE_TYPE_ZIP;

    case 0x6d736100:  // WASM
        return FILE_TYPE_WASM;

    case 0x89504E47:  // PNG
        return FILE_TYPE_PNG;

    // JPEG variants
    case 0xe0ffd8ff:  // JPEG/JFIF
    case 0xe1ffd8ff:  // JPEG/Exif
    case 0xeeffd8ff:  // JPEG/SPIFF
    case 0xfeffd8ff:  // JPEG/COM
        return FILE_TYPE_JPEG;

    case 0x46464952:  // WAV ("RIFF")
        return FILE_TYPE_WAV;

    case 0x5367674f:  // OGG ("OggS")
        return FILE_TYPE_OGG;

    // MP3 variants
    case 0x03334449:  // MP3 with ID3v2.3
    case 0x02334449:  // MP3s with ID3v2.2
    case 0x04334449:  // MP3 with ID3v2.4
        return FILE_TYPE_MP3;

    default:
        return FILE_TYPE_UNKNOWN;
  }
}

// detect file-type from native filesystem file
DetectFileType fs_detect_type_real(char* filename) {
  struct stat sb;

  if (stat(filename, &sb) != 0){
    return FILE_TYPE_UNKNOWN;
  }
  if (S_ISDIR(sb.st_mode)) {
    return FILE_TYPE_DIR;
  }

  uint32_t magic_number = 0;
  FILE* file = fopen(filename, "rb");
  if (!file) {
    return FILE_TYPE_UNKNOWN;
  }
  fread(&magic_number, sizeof(uint32_t), 1, file);
  fclose(file);

  return fs_parse_magic_bytes(magic_number);
}

// detect file-type from physfs filesystem file
DetectFileType fs_detect_type(char* filename) {
  // TODO
  return FILE_TYPE_UNKNOWN;
}


// get the short-name of cart, using filename
char* fs_get_cart_name(char* filename) {
  // TODO
  return NULL;
}

// Get info about a file from native filesystem
PHYSFS_Stat fs_file_info(char* filename) {
  PHYSFS_Stat stat;
  PHYSFS_stat(filename, &stat);
  return stat;
}
