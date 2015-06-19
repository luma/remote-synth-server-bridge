#ifndef SYNTH_BRIDGE_COMMON_V8_H_
#define SYNTH_BRIDGE_COMMON_V8_H_
#pragma once

#include <string>
#include <node.h>
#include <uv.h>
#include "common/Logging.h"

typedef v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> PersistentContext;
typedef v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> PersistentFunction;
typedef const v8::FunctionCallbackInfo<v8::Value>& FunctionArgs;
typedef const v8::PropertyCallbackInfo<v8::Value>& PropertyInfo;

namespace V8Helpers {
  void CallFn(PersistentFunction fn, unsigned int argc, v8::Handle<v8::Value> argv[]);
  std::string CoerceFromV8Str(v8::Local<v8::Value> str);
  v8::Local<v8::String> CoerceToV8Str(std::string str);
}


// libuv mutex lock with RAII
//
// Usage:
//  {
//    UVLock uvLock(lock_);
//    // ... do awesome stuff!!
//
//    // uvLock is released for you here
//  }
//
class UVLock {
 public:
  explicit UVLock(uv_mutex_t lock) : lock_(lock) {
    uv_mutex_lock(&lock_);
  }

  ~UVLock() {
    uv_mutex_unlock(&lock_);
  }

 private:
  uv_mutex_t lock_;
};

#endif    // SYNTH_BRIDGE_COMMON_V8_H_
