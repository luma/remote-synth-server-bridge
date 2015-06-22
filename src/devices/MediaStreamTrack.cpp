#include "devices/MediaStreamTrack.h"

v8::Persistent<v8::Function> MediaStreamTrack::constructor;

MediaStreamTrack::MediaStreamTrack(webrtc::MediaStreamTrackInterface* track)
  : track_(track) {}

MediaStreamTrack::~MediaStreamTrack() {
  track_ = nullptr;
}

void MediaStreamTrack::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MediaStreamTrack"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "id"), GetId, nullptr);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "kind"), GetKind, nullptr);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "enabled"), GetEnabled, SetEnabled);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "state"), GetState, nullptr);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(
      v8::String::NewFromUtf8(isolate, "MediaStreamTrack"), tpl->GetFunction());
}

void MediaStreamTrack::New(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MediaStreamTrack(...)`
    v8::Local<v8::External> _track = v8::Local<v8::External>::Cast(args[0]);
    auto track = static_cast<webrtc::MediaStreamTrackInterface*>(_track->Value());

    MediaStreamTrack* obj = new MediaStreamTrack(track);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MediaStreamTrack(...)`, turn into
    // construct call.
    // @todo argument
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = {};

    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

v8::Local<v8::Object> MediaStreamTrack::ToWrapped(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = {
    v8::External::New(isolate, static_cast<void*>(track.get()))
  };

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);
  return scope.Escape(instance);
}

void MediaStreamTrack::GetId(v8::Local<v8::String> property, PropertyInfo info) {
  auto value = ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder())->track_->id();
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaStreamTrack::GetKind(v8::Local<v8::String> property, PropertyInfo info) {
  auto value = ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder())->GetKind();
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaStreamTrack::GetState(v8::Local<v8::String> property, PropertyInfo info) {
  auto value = ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder())->StateToString();
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaStreamTrack::GetEnabled(v8::Local<v8::String> property, PropertyInfo info) {
  auto isolate = v8::Isolate::GetCurrent();
  auto value = ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder())->track_->enabled();
  info.GetReturnValue().Set(v8::Boolean::New(isolate, value));
}

void MediaStreamTrack::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
  auto self = ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->track_->set_enabled(value->BooleanValue() == true);
}

// Converts track_.state() (i.e. MediaStreamTrackInterface::TrackState)
// to a cstring.
const char* MediaStreamTrack::StateToString() {
  switch (track_->state()) {
    case webrtc::MediaStreamTrackInterface::kInitializing:
      // Track is being negotiated.
      return "initializing";

    case webrtc::MediaStreamTrackInterface::kLive:
      // Track alive
      return "live";

    case webrtc::MediaStreamTrackInterface::kEnded:
      // Track have ended
      return "ended";

    case webrtc::MediaStreamTrackInterface::kFailed:
      // Track negotiation failed.
      return "failed";
  }
}
