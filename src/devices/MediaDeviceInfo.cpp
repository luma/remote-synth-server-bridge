#include "devices/MediaDeviceInfo.h"
#include "common/V8.h"

v8::Persistent<v8::Function> MediaDeviceInfo::constructor;

// The external object is simply a wrapper around a void*. External objects can
// only be used to store reference values in internal fields. JavaScript objects
// can not have references to C++ objects directly so the external value is used
// as a "bridge" to go from JavaScript into C++.  In that sense external values
// are the opposite of handles since handles lets C++ make references to
// JavaScript objects.
//
// https://developers.google.com/v8/embed
//
MediaDeviceInfo* GetFromPropertyCallbackInfo(const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto self = info.Holder();
  auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
  return static_cast<MediaDeviceInfo*>(wrap->Value());
}


MediaDeviceInfo::MediaDeviceInfo(
  std::string id, std::string kind, std::string label, std::string groupId)
  : id_(id), kind_(kind), label_(label), groupId_(groupId) {}

void MediaDeviceInfo::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MediaDeviceInfo"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "deviceId"), GetId, NULL);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "kind"), GetKind, NULL);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "label"), GetLabel, NULL);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "groupId"), GetGroupId, NULL);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(
      v8::String::NewFromUtf8(isolate, "MediaDeviceInfo"), tpl->GetFunction());
}

void MediaDeviceInfo::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MediaDeviceInfo(...)`
    std::string id = V8Helpers::CoerceFromV8Str(args[0]);
    std::string kind = V8Helpers::CoerceFromV8Str(args[1]);
    std::string label = V8Helpers::CoerceFromV8Str(args[2]);
    std::string groupId = V8Helpers::CoerceFromV8Str(args[3]);

    MediaDeviceInfo* obj = new MediaDeviceInfo(id, kind, label, groupId);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MediaDeviceInfo(...)`, turn into
    // construct call.
    const int argc = 4;
    v8::Local<v8::Value> argv[argc] = {
      args[0],
      args[1],
      args[2],
      args[3]
    };

    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void MediaDeviceInfo::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto DefaultStr = [&isolate](v8::Local<v8::Value> str) {
    return str->IsUndefined() ? v8::String::Empty(isolate) : str->ToString();
  };

  const unsigned argc = 4;
  v8::Handle<v8::Value> argv[argc] = {
    DefaultStr(args[0]),
    DefaultStr(args[1]),
    DefaultStr(args[2]),
    DefaultStr(args[3])
  };

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

v8::Local<v8::Object> MediaDeviceInfo::ToWrapped(std::string kind, const cricket::Device &device) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  const int argc = 4;
  v8::Local<v8::Value> argv[argc] = {
    V8Helpers::CoerceToV8Str(device.id),
    V8Helpers::CoerceToV8Str(kind),
    V8Helpers::CoerceToV8Str(device.name),
    v8::String::Empty(isolate)
  };

  auto cons = v8::Local<v8::Function>::New(isolate, MediaDeviceInfo::constructor);
  auto instance = cons->NewInstance(argc, argv);
  return scope.Escape(instance);
}

void MediaDeviceInfo::GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<MediaDeviceInfo>(info.Holder())->id_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaDeviceInfo::GetKind(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<MediaDeviceInfo>(info.Holder())->kind_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaDeviceInfo::GetLabel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<MediaDeviceInfo>(info.Holder())->label_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaDeviceInfo::GetGroupId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto value = ObjectWrap::Unwrap<MediaDeviceInfo>(info.Holder())->groupId_;
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}
