#pragma once

#include <cstdio>


template <class T>
class Error {
public:
  static T* handle(const char *message) {
    fprintf(stderr, "Error, %s\n", message);
    return nullptr;
  };
};
