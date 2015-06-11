#include "devices/MediaDevices.h"
#include "common/V8.h"

v8::Persistent<v8::Function> MediaDevices::constructor;

MediaDevices::MediaDevices() {
  deviceManager_.reset(cricket::DeviceManagerFactory::Create());

  if (false == deviceManager_->Init()) {
    ERROR("Cannot create device manager");
    // @todo handle this better
    return;
  }
}

MediaDevices::~MediaDevices() {
  if (deviceManager_) {
    deviceManager_->Terminate();
  }

  deviceManager_ = nullptr;
}

void MediaDevices::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MediaDevices"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "enumerateDevices", EnumerateDevices);

  tpl->InstanceTemplate()->SetAccessor(
              v8::String::NewFromUtf8(isolate, "ondevicechange"),
              GetOnDeviceChange,
              SetOnDeviceChange);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(v8::String::NewFromUtf8(isolate, "MediaDevices"),
               tpl->GetFunction());
}

void MediaDevices::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MediaDevices(...)`
    MediaDevices* obj = new MediaDevices();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MediaDevices(...)`, turn into
    // construct call.
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = {};
    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void MediaDevices::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  const unsigned argc = 0;
  v8::Handle<v8::Value> argv[argc] = {};

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void MediaDevices::GetOnDeviceChange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
  auto isolate = v8::Isolate::GetCurrent();
  auto value = ObjectWrap::Unwrap<MediaDevices>(info.Holder())->onDeviceChange_;
  info.GetReturnValue().Set(v8::Local<v8::Function>::New(isolate, value));
}

void MediaDevices::SetOnDeviceChange(v8::Local<v8::String> property,
      v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
  // v8::Local<v8::Object> self = info.Holder();
  // v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
  // void* ptr = wrap->Value();
  // static_cast<MediaDevices*>(ptr)->onDeviceChange_ = value->Int32Value();

  auto isolate = v8::Isolate::GetCurrent();
  auto self = ObjectWrap::Unwrap<MediaDevices>(info.Holder());

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  self->onDeviceChange_  = PersistentFunction::Persistent(
                          isolate, v8::Handle<v8::Function>::Cast(value));
}


void MediaDevices::EnumerateDevices(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaDevices>(args.Holder());

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

  self->eventLoop_.CallAsync([self, &hasVideo, &hasAudio, &callback](void* data) {
    self->OnEnumerateDevices(hasVideo, hasAudio, callback);
  });

  args.GetReturnValue().Set(args.This());
}

void MediaDevices::OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback) {
  INFO("MediaDevices::OnEnumerateDevices");

  std::vector<cricket::Device> audioDevices;
  std::vector<cricket::Device> videoDevices;

  if (hasAudio) {
    INFO("Enumerating Audio devices");
    deviceManager_->GetAudioInputDevices(&audioDevices);
  }

  if (hasVideo) {
    INFO("Enumerating Video devices");
    deviceManager_->GetVideoCaptureDevices(&videoDevices);
  }

  // build js friendly object
  {
    auto isolate = v8::Isolate::GetCurrent();
    const unsigned argc = 2;
    // HandleScope scope(isolate);

    size_t deviceIndex = 0;
    auto devices = v8::Array::New(isolate,
                      audioDevices.size() + videoDevices.size());

    for (auto const & device: audioDevices) {
      INFO("Audio Device: %s", device.name.c_str());
      devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("audioinput", device));
      deviceIndex++;
    }

    for (auto const & device: videoDevices) {
      INFO("Video Device: %s", device.name.c_str());
      devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("videoinput", device));
      deviceIndex++;
    }

    v8::Local<v8::Value> argv[argc] = { v8::Undefined(isolate), devices };
    V8Helpers::CallFn(callback, argc, argv);

    // @todo free up event?
  }
}
