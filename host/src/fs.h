// this implements shared filesystem functions

#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "physfs.h"

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

#ifdef FS_IMPLEMENTATION

// call this to initialize filesystem
bool fs_init(char* cartFilename) {
  // logic:
  //   mount root & write-dir (per-cartname)
  //   if cartName=dir: mount dir
  //   if cartName=zip: mount zip
  //   if cartName=wasm: mount dir that contains the wasm
  //   else: return false
  //
  char* cartName = fs_get_cart_name(cartFilename);

  if (cartName == NULL) {
    return false;
  }

  if (!PHYSFS_init("/")) {
    return false;
  }

  const char* null0_writable_dir = PHYSFS_getPrefDir("null0", cartName);

  // build what is put into null0_writable_dir, so it can be mounted
  #ifdef EMSCRIPTEN
    if (mkdir("/home", 0) == -1) {
      // printf("could not make /home\n");
    }
    if (mkdir("/home/web_user", 0) == -1) {
      // printf("could not make /home/web_user\n");
    }
    if (mkdir("/home/web_user/.local", 0) == -1) {
      // printf("could not make /home/web_user/.local\n");
    }
    if (mkdir("/home/web_user/.local/share", 0) == -1) {
      // printf("could not make /home/web_user/.local/share\n");
    }
    if (mkdir(null0_writable_dir, 0) == -1) {
      // printf("could not make %s\n", null0_writable_dir);
    }
  #endif


  if (null0_writable_dir == NULL) {
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
      break;
    }
    case FILE_TYPE_WASM: {
      if (!PHYSFS_mount(dirname(cartFilename), NULL, 1)) {
        PHYSFS_deinit();
        return false;
      }
      break;
    }
    default: return false;
  }

  if (!PHYSFS_mount(null0_writable_dir, NULL, 1)) {
    PHYSFS_deinit();
    return false;
  }

  if (!PHYSFS_setWriteDir((const char*)null0_writable_dir)) {
    PHYSFS_deinit();
    return false;
  }

  return true;
}

// called to unload filesystem
void fs_unload() {
  PHYSFS_deinit();
}

// load a file from native filesystem
unsigned char* fs_load_file_real(char *filename, unsigned int *bytesRead) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    *bytesRead = 0;
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  if (size <= 0) {
    fclose(file);
    *bytesRead = 0;
    return NULL;
  }

  unsigned char* data = (unsigned char*)malloc(size * sizeof(unsigned char));
  if (data == NULL) {
    fclose(file);
    *bytesRead = 0;
    return NULL;
  }

  // Read the file
  unsigned int bytes = (unsigned int)fread(data, sizeof(unsigned char), size, file);
  fclose(file);
  *bytesRead = bytes;

  return data;
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
  if (filename == NULL || data == NULL) {
    return false;
  }
  FILE *file = fopen(filename, "wb");
  if (file == NULL) {
    return false;
  }

  size_t count = fwrite(data, sizeof(unsigned char), byteSize, file);

  if (count <= 0) {
    fclose(file);
    return false;
  }

  if (count != (size_t)byteSize) {
    fclose(file);
    return false;
  }

  return fclose(file) == 0;
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
  PHYSFS_File* f = PHYSFS_openRead(filename);
  if (!f) {
    return FILE_TYPE_UNKNOWN;
  }
  uint32_t magic_number = 0;
  PHYSFS_sint64 br = PHYSFS_readBytes(f, (unsigned char*)&magic_number, sizeof(uint32_t));
  if (br != sizeof(uint32_t)) {
    return FILE_TYPE_UNKNOWN;
  }
  PHYSFS_close(f);
  return fs_parse_magic_bytes(magic_number);
}


char* fs_get_cart_name(char* filename) {
    // Allocate memory that will persist after function returns
    char* sname = strdup(filename);  // Make a copy of filename
    if (!sname) return NULL;

    char* bname = basename(sname);
    char* cartName = strtok(bname, ".");

    if (!cartName || strlen(cartName) > 127) {
        free(sname);
        return NULL;
    }

    // Make a copy of cartName before freeing sname
    char* result = strdup(cartName);
    free(sname);

    return result;
}

// Get info about a file from native filesystem
PHYSFS_Stat fs_file_info(char* filename) {
  PHYSFS_Stat stat;
  PHYSFS_stat(filename, &stat);
  return stat;
}

#endif // FS_IMPLEMENTATION
