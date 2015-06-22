#include <memory>
#include "talk/app/webrtc/videosourceinterface.h"
#include "common/Guid.h"
#include "common/V8.h"
#include "devices/MediaStream.h"
#include "devices/MediaDevicesWrapper.h"

v8::Persistent<v8::Function> MediaDevicesWrapper::constructor;

MediaDevicesWrapper::MediaDevicesWrapper() {
}

MediaDevicesWrapper::~MediaDevicesWrapper() {
  eventLoop_.Terminate();
}

void MediaDevicesWrapper::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MediaDevices"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "enumerateDevices", EnumerateDevices);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getMedia", GetMedia);


  tpl->InstanceTemplate()->SetAccessor(
              v8::String::NewFromUtf8(isolate, "ondevicechange"),
              GetOnDeviceChange,
              SetOnDeviceChange);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(v8::String::NewFromUtf8(isolate, "MediaDevices"),
               tpl->GetFunction());
}

void MediaDevicesWrapper::New(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MediaDevicesWrapper(...)`
    MediaDevicesWrapper* obj = new MediaDevicesWrapper();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MediaDevicesWrapper(...)`, turn into
    // construct call.
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = {};
    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void MediaDevicesWrapper::NewInstance(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  const unsigned argc = 0;
  v8::Handle<v8::Value> argv[argc] = {};

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void MediaDevicesWrapper::GetOnDeviceChange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
  auto isolate = v8::Isolate::GetCurrent();
  auto value = ObjectWrap::Unwrap<MediaDevicesWrapper>(info.Holder())->onDeviceChange_;
  info.GetReturnValue().Set(v8::Local<v8::Function>::New(isolate, value));
}

void MediaDevicesWrapper::SetOnDeviceChange(v8::Local<v8::String> property,
      v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {

  auto isolate = v8::Isolate::GetCurrent();
  auto self = ObjectWrap::Unwrap<MediaDevicesWrapper>(info.Holder());

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  self->onDeviceChange_  = PersistentFunction::Persistent(
                          isolate, v8::Handle<v8::Function>::Cast(value));
}

void MediaDevicesWrapper::EnumerateDevices(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaDevicesWrapper>(args.Holder());

  // Turn a JS object that looks like {foo: true, bar: false, baz: true}
  // in a std::vector like ["foo", "baz"]
  //
  v8::Local<v8::Object> types = v8::Local<v8::Object>::Cast(args[0]);
  v8::Local<v8::Value> audioValue = types->Get(v8::String::NewFromUtf8(isolate, "audio"));
  v8::Local<v8::Value> videoValue = types->Get(v8::String::NewFromUtf8(isolate, "video"));
  bool hasAudio = audioValue->IsBoolean() && audioValue->BooleanValue() == true;
  bool hasVideo = videoValue->IsBoolean() && videoValue->BooleanValue() == true;

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  auto callback = PersistentFunction::Persistent(isolate,
                                    v8::Handle<v8::Function>::Cast(args[1]));

  self->eventLoop_.CallAsync([
    self, hasVideo, hasAudio,
    callback = std::move(callback)
  ](void* data) {
    self->OnEnumerateDevices(hasVideo, hasAudio, callback);
  });

  args.GetReturnValue().Set(args.This());
}

void MediaDevicesWrapper::OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback) {
  INFO("MediaDevicesWrapper::OnEnumerateDevices");

  DeviceCollection audioDevices;
  DeviceCollection videoDevices;

  if (hasAudio) {
    INFO("Enumerating Audio devices");
    mediaDevices_.GetCaptureDevices(MediaDevices::AUDIO, &audioDevices);
  }

  if (hasVideo) {
    INFO("Enumerating Video devices");
    mediaDevices_.GetCaptureDevices(MediaDevices::VIDEO, &videoDevices);
  }

  // build js friendly object
  {
    auto devices = WrapNativeDevices(audioDevices, videoDevices);
    const unsigned argc = 2;
    auto isolate = v8::Isolate::GetCurrent();
    // HandleScope scope(isolate);
    v8::Local<v8::Value> argv[argc] = { v8::Undefined(isolate), devices };
    V8Helpers::CallFn(callback, argc, argv);

    // @todo free up event?
  }
}

//
// MediaDevices.getMedia({
//   video: "DeviceId", audio: "DeviceId"
// }, function(err, mediaDevice) {
//
// });
//
void MediaDevicesWrapper::GetMedia(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaDevicesWrapper>(args.Holder());
  auto types = v8::Local<v8::Object>::Cast(args[0]);
  v8::Local<v8::Value> audioDeviceId = types->Get(v8::String::NewFromUtf8(isolate, "audio"));
  v8::Local<v8::Value> videoDeviceId = types->Get(v8::String::NewFromUtf8(isolate, "video"));

  MediaConstraints constraints;
  if (audioDeviceId->IsString()) {
    constraints["audio"] = V8Helpers::CoerceFromV8Str(audioDeviceId->ToString());
  }

  if (videoDeviceId->IsString()) {
    constraints["video"] = V8Helpers::CoerceFromV8Str(videoDeviceId->ToString());
  }

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  auto callback = PersistentFunction::Persistent(isolate,
                                    v8::Handle<v8::Function>::Cast(args[1]));

  self->eventLoop_.CallAsync([
    self, constraints,
    callback = std::move(callback)
  ](void* data) {
    self->OnGetMedia(constraints, callback);
  });

  args.GetReturnValue().Set(args.This());
}

void MediaDevicesWrapper::OnGetMedia(MediaConstraints constraints, PersistentFunction callback) {
  INFO("MediaDevicesWrapper::OnGetMedia");
  auto stream = mediaDevices_.GetMedia(constraints);

  if (stream) {
    auto wrappedStream = MediaStream::ToWrapped(stream);

    const unsigned argc = 2;
    auto isolate = v8::Isolate::GetCurrent();
    // HandleScope scope(isolate);
    v8::Local<v8::Value> argv[argc] = { v8::Undefined(isolate), wrappedStream };
    V8Helpers::CallFn(callback, argc, argv);
  }
}

v8::Local<v8::Array> MediaDevicesWrapper::WrapNativeDevices(DeviceCollection audio, DeviceCollection video) {
  auto isolate = v8::Isolate::GetCurrent();
  // HandleScope scope(isolate);

  size_t deviceIndex = 0;
  auto devices = v8::Array::New(isolate, audio.size() + video.size());

  for (auto const & device: audio) {
    INFO("Audio Device: %s", device.name.c_str());
    devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("audioinput", device));
    deviceIndex++;
  }

  for (auto const & device: video) {
    INFO("Video Device: %s", device.name.c_str());
    devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("videoinput", device));
    deviceIndex++;
  }

  return devices;
}

