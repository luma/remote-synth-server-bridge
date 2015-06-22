#include <memory>
#include "talk/app/webrtc/videosourceinterface.h"
#include "devices/MediaDevices.h"
#include "common/Guid.h"
#include "common/V8.h"

v8::Persistent<v8::Function> MediaDevices::constructor;

MediaDevices::MediaDevices()
  : pcWorkerThread_(new rtc::Thread()),
    pcSignalingThread_(new rtc::Thread()) {

  assert(NULL != pcSignalingThread_ && NULL != pcWorkerThread_);

  pcWorkerThread_->Start();
  pcSignalingThread_->Start();

  audioDevice_ = webrtc::CreateAudioDeviceModule(0,
                    webrtc::AudioDeviceModule::kPlatformDefaultAudio);

  if (audioDevice_->Init() < 0) {
    ERROR("Cannot create audio device module");
    // @todo handle this better
    return;
  }

  pcFactory_ = webrtc::CreatePeerConnectionFactory(
          pcWorkerThread_, pcSignalingThread_, audioDevice_, nullptr, nullptr);

  if (!pcFactory_.get()) {
    ERROR("Could not create PeerConnectionFactory");
    // @todo some crazy shit
    return;
  }

  deviceManager_.reset(cricket::DeviceManagerFactory::Create());

  if (false == deviceManager_->Init()) {
    ERROR("Cannot create device manager");
    // @todo handle this better
    return;
  }
}

MediaDevices::~MediaDevices() {
  eventLoop_.Terminate();

  if (deviceManager_) {
    deviceManager_->Terminate();
  }

  deviceManager_ = nullptr;
  audioDevice_ = nullptr;
  pcFactory_ = nullptr;

  pcWorkerThread_->Stop();
  pcWorkerThread_->Quit();
  pcSignalingThread_->Stop();
  pcSignalingThread_->Quit();
  delete pcWorkerThread_;
  delete pcSignalingThread_;
}

void MediaDevices::Init(v8::Handle<v8::Object> exports) {
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

void MediaDevices::New(FunctionArgs args) {
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

void MediaDevices::NewInstance(FunctionArgs args) {
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

  auto isolate = v8::Isolate::GetCurrent();
  auto self = ObjectWrap::Unwrap<MediaDevices>(info.Holder());

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  self->onDeviceChange_  = PersistentFunction::Persistent(
                          isolate, v8::Handle<v8::Function>::Cast(value));
}


void MediaDevices::EnumerateDevices(FunctionArgs args) {
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

  self->eventLoop_.CallAsync([
    self, hasVideo, hasAudio,
    callback = std::move(callback)
  ](void* data) {
    self->OnEnumerateDevices(hasVideo, hasAudio, callback);
  });

  args.GetReturnValue().Set(args.This());
}

//
// MediaDevices.getMedia({
//   video: "DeviceId", audio: "DeviceId"
// }, function(err, mediaDevice) {
//
// });
//
void MediaDevices::GetMedia(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaDevices>(args.Holder());
  auto types = v8::Local<v8::Object>::Cast(args[0]);
  v8::Local<v8::Value> audioValueId = types->Get(v8::String::NewFromUtf8(isolate, "audio"));
  v8::Local<v8::Value> videoValueId = types->Get(v8::String::NewFromUtf8(isolate, "video"));
  auto audioDeviceId = audioValueId->IsStringObject() ? V8Helpers::CoerceFromV8Str(audioValueId->ToString()) : "";
  auto videoDeviceId = videoValueId->IsStringObject() ? V8Helpers::CoerceFromV8Str(videoValueId->ToString()) : "";

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  auto callback = PersistentFunction::Persistent(isolate,
                                    v8::Handle<v8::Function>::Cast(args[1]));

  self->eventLoop_.CallAsync([
    self, audioDeviceId, videoDeviceId,
    callback = std::move(callback)
  ](void* data) {
    self->OnGetMedia(audioDeviceId, videoDeviceId, callback);
  });

  args.GetReturnValue().Set(args.This());
}

void MediaDevices::OnGetMedia(std::string audioDeviceId, std::string videoDeviceId, PersistentFunction callback) {
  INFO("MediaDevices::OnGetMedia");

  auto kAudioLabel  = NewGuidStr();
  auto kVideoLabel  = NewGuidStr();
  auto kStreamLabel = NewGuidStr();
  auto stream = pcFactory_->CreateLocalMediaStream(kStreamLabel);

  if (!audioDeviceId.empty()) {
    INFO("Creating Audio track with %s device", audioDeviceId.c_str());

    audioDevice_->SetRecordingDevice(atoi(audioDeviceId.c_str()));
    audioDevice_->InitRecording();
    audioDevice_->StartRecording();

    stream->AddTrack(pcFactory_->CreateAudioTrack(kAudioLabel, nullptr));
  }

  if (!videoDeviceId.empty()) {
    INFO("Creating Video track with %s device", videoDeviceId.c_str());
    auto capturer = GetCapturerById(VIDEO, videoDeviceId);
    if (capturer == nullptr) {
      // @todo handle error
      return;
    }

    rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack(
      pcFactory_->CreateVideoTrack(kVideoLabel,
        pcFactory_->CreateVideoSource(capturer, nullptr))
    );

    stream->AddTrack(videoTrack);
  }

  // @todo wrap up a MediaStream for Node
  // @todo pass it to the callback
}

cricket::VideoCapturer* MediaDevices::GetCapturerById(Type type, std::string deviceId) {
  DeviceCollection devices;

  if (!GetCaptureDevices(type, &devices)) {
    // @todo probably handle this better
    return nullptr;
  }

  auto it = std::find_if(devices.begin(), devices.end(), [deviceId] (const auto& device) {
    return device.id == deviceId;
  });

  if (it == devices.end()) {
    return nullptr;
  }

  return deviceManager_->CreateVideoCapturer(*it);
}

bool MediaDevices::GetCaptureDevices(Type type, DeviceCollection* devices) {
  switch (type) {
  case VIDEO:
    if (!deviceManager_->GetVideoCaptureDevices(devices)) {
      ERROR("Could not get media capturers for audio devices");
      return false;
    }
    INFO("Got audio devices");
    break;

  case AUDIO:
    if (!deviceManager_->GetAudioInputDevices(devices)) {
      ERROR("Could not get media capturers for video devices");
      return false;
    }
    break;

  default:
    ERROR("Could not get media capturers for unknown device type: %d", static_cast<int>(type));
    return false;
  }

  return true;
}

void MediaDevices::OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback) {
  INFO("MediaDevices::OnEnumerateDevices");

  DeviceCollection audioDevices;
  DeviceCollection videoDevices;

  if (hasAudio) {
    INFO("Enumerating Audio devices");
    GetCaptureDevices(AUDIO, &audioDevices);
  }

  if (hasVideo) {
    INFO("Enumerating Video devices");
    GetCaptureDevices(VIDEO, &videoDevices);
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

v8::Local<v8::Array> MediaDevices::WrapNativeDevices(DeviceCollection audio, DeviceCollection video) {
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
