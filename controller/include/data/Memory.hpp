#pragma once

#include <memory>
#include <Arduino.h>


class RTOSAlloc {
public:
  typedef char* pointer;
  typedef const char* const_pointer;
  typedef char value_type;

  void* allocate(size_t len) {
    return malloc(len);
  }

  void deallocate(char *ptr) {
    free(ptr)
  }
} rtos_alloc;