#ifndef SYNTH_BRIDGE_LOGGING_H_
#define SYNTH_BRIDGE_LOGGING_H_
#pragma once

#include <memory>
#include <string>
#include "common/Logging.h"

template<typename ... Args>
inline  std::string stringFormat(const char* format, Args... args) {
  // Extra space for '\0'
  size_t size = snprintf(nullptr, 0, format, args...) + 1;
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format, args...);

  // and now trim the final '\0'
  return std::string(buf.get(), buf.get() + size - 1);
}

inline void WARN(const char* message) {
  fprintf(stdout, "\033[01;33m native:%s \033[00m\n", message);
}

inline void WARN(std::string message) {
  WARN(message.c_str());
}

template<typename ... Args>
inline void WARN(const char* format, Args... args) {
  WARN(stringFormat(format, args...));
}

inline void ERROR(const char* message) {
  fprintf(stdout, "\033[01;32m native:%s \033[00m\n", message);
}

inline void ERROR(std::string message) {
  ERROR(message.c_str());
}

template<typename ... Args>
inline void ERROR(const char* format, Args... args) {
  ERROR(stringFormat(format, args...));
}

inline void INFO(const char* message) {
  fprintf(stdout, "\033[01;34m native:%s \033[00m\n", message);
}

inline void INFO(std::string message) {
  INFO(message.c_str());
}

template<typename ... Args>
inline void INFO(const char* format, Args... args) {
  INFO(stringFormat(format, args...));
}

#endif    // SYNTH_BRIDGE_LOGGING_H_
