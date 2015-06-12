#include "negotiation/IceCandidate.h"

v8::Persistent<v8::Function> IceCandidate::constructor;

void IceCandidate::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "IceCandidate"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "sdpMid"), GetSdpMid, NULL);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "sdpMLineIndex"), GetSdpMLineIndex, NULL);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "candidate"), GetCandidate, NULL);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(
      v8::String::NewFromUtf8(isolate, "IceCandidate"), tpl->GetFunction());
}

void IceCandidate::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor:
    //   `new IceCandidate(sdpMid, sdpMLineIndex, candidate)`
    //
    std::string sdpMid = V8Helpers::CoerceFromV8Str(args[0]);
    int sdpMLineIndex = args[0]->Int32Value();
    std::string candidate = V8Helpers::CoerceFromV8Str(args[1]);

    auto obj = new IceCandidate(sdpMid, sdpMLineIndex, candidate);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function:
    //   `IceCandidate(sdpMid, sdpMLineIndex, candidate)`
    // turn into construct call.
    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      args[0],
      args[1],
      args[2]
    };

    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void IceCandidate::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto DefaultStr = [&isolate](v8::Local<v8::Value> str) {
    return str->IsUndefined() ? v8::String::Empty(isolate) : str->ToString();
  };

  const unsigned argc = 3;
  v8::Handle<v8::Value> argv[argc] = {
    DefaultStr(args[0]),
    args[1],
    DefaultStr(args[2])
  };

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

v8::Local<v8::Object> IceCandidate::ToWrapped(std::string sdpMid, int sdpMLineIndex, std::string candidate) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  const int argc = 4;
  v8::Local<v8::Value> argv[argc] = {
    V8Helpers::CoerceToV8Str(sdpMid),
    v8::Number::New(isolate, sdpMLineIndex),
    V8Helpers::CoerceToV8Str(candidate)
  };

  auto cons = v8::Local<v8::Function>::New(isolate, IceCandidate::constructor);
  auto instance = cons->NewInstance(argc, argv);
  return scope.Escape(instance);
}

void IceCandidate::GetSdpMid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<IceCandidate>(info.Holder())->sdpMid_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void IceCandidate::GetSdpMLineIndex(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<IceCandidate>(info.Holder())->sdpMLineIndex_;
  info.GetReturnValue().Set(value);
}

void IceCandidate::GetCandidate(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<IceCandidate>(info.Holder())->candidate_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}
