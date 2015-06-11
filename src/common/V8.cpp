#include "common/V8.h"

namespace V8Helpers {

void CallFn(PersistentFunction fn, unsigned int argc, v8::Handle<v8::Value> argv[]) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();

  // Invoke the process function, giving the global object as 'this'
  // and whatever arguments were passed in
  auto callableFn = v8::Local<v8::Function>::New(isolate, fn);

  {
    // Set up an exception handler before calling the handler function
    v8::TryCatch tryCatch;

    callableFn->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    if (tryCatch.HasCaught()) {
      v8::Local<v8::Value> exception = tryCatch.Exception();
      v8::String::Utf8Value exceptionStr(exception);
      ERROR("Exception: %s\n", *exceptionStr);
    }
  }
}

std::string CoerceFromV8Str(v8::Local<v8::Value> str) {
  if (str->IsUndefined()) {
    return std::string();
  }

  v8::String::Utf8Value utf8Str(str->ToString());
  return std::string(*utf8Str);
}

v8::Local<v8::String> CoerceToV8Str(std::string str) {
  auto isolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(
            isolate, str.c_str(), v8::String::kNormalString, str.length());
}

}   // namespace V8Helpers
