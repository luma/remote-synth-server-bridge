#include "devices/MediaStreamTrack.h"
#include "devices/MediaStream.h"

v8::Persistent<v8::Function> MediaStream::constructor;

MediaStream::MediaStream(webrtc::MediaStreamInterface* stream)
  : stream_(stream) {}

MediaStream::~MediaStream() {
  stream_ = nullptr;
}

void MediaStream::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MediaStream"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "addTrack", AddTrack);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeTrack", RemoveTrack);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findAudioTrack", FindAudioTrack);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findVideoTrack", FindVideoTrack);

  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "label"), GetLabel, nullptr);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "audioTrack"), GetAudioTracks, nullptr);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "videoTracks"), GetVideoTracks, nullptr);

  constructor.Reset(isolate, tpl->GetFunction());

  exports->Set(
      v8::String::NewFromUtf8(isolate, "MediaStream"), tpl->GetFunction());
}

void MediaStream::New(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MediaStream(...)`
    v8::Local<v8::External> _stream = v8::Local<v8::External>::Cast(args[0]);
    auto stream = static_cast<webrtc::MediaStreamInterface*>(_stream->Value());

    MediaStream* obj = new MediaStream(stream);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MediaStream(...)`, turn into
    // construct call.
    // @todo argument
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = {};

    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

v8::Local<v8::Object> MediaStream::ToWrapped(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = {
    v8::External::New(isolate, static_cast<void*>(stream.get()))
  };

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);
  return scope.Escape(instance);
}

void MediaStream::AddTrack(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaStream>(args.Holder());
  auto _track = ObjectWrap::Unwrap<MediaStreamTrack>(args[0]->ToObject());
  auto kind = _track->GetKind();
  bool success = false;

  if (kind == "audio") {
    auto track = static_cast<webrtc::AudioTrackInterface*>(_track->Value());
    success = self->stream_->AddTrack(track);
  } else if (kind == "video") {
    auto track = static_cast<webrtc::VideoTrackInterface*>(_track->Value());
    success = self->stream_->AddTrack(track);
  }

  args.GetReturnValue().Set(v8::Boolean::New(isolate, success));
}

void MediaStream::RemoveTrack(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaStream>(args.Holder());
  auto _track = ObjectWrap::Unwrap<MediaStreamTrack>(args[0]->ToObject());
  auto kind = _track->GetKind();
  bool success = false;

  if (kind == "audio") {
    auto track = static_cast<webrtc::AudioTrackInterface*>(_track->Value());
    success = self->stream_->RemoveTrack(track);
  } else if (kind == "video") {
    auto track = static_cast<webrtc::VideoTrackInterface*>(_track->Value());
    success = self->stream_->RemoveTrack(track);
  }

  args.GetReturnValue().Set(v8::Boolean::New(isolate, success));
}

void MediaStream::FindAudioTrack(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaStream>(args.Holder());
  auto _trackId = v8::Handle<v8::String>::Cast(args[0])->ToString();
  auto trackId = V8Helpers::CoerceFromV8Str(_trackId);
  auto track = self->stream_->FindAudioTrack(trackId);

  args.GetReturnValue().Set(MediaStreamTrack::ToWrapped(track));
}

void MediaStream::FindVideoTrack(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<MediaStream>(args.Holder());
  auto _trackId = v8::Handle<v8::String>::Cast(args[0])->ToString();
  auto trackId = V8Helpers::CoerceFromV8Str(_trackId);
  auto track = self->stream_->FindVideoTrack(trackId);

  args.GetReturnValue().Set(MediaStreamTrack::ToWrapped(track));
}

void MediaStream::GetLabel(v8::Local<v8::String> property, PropertyInfo info) {
  auto value = ObjectWrap::Unwrap<MediaStream>(info.Holder())->stream_->label();
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void MediaStream::GetAudioTracks(v8::Local<v8::String> property, PropertyInfo info) {
  auto nativeTracks = ObjectWrap::Unwrap<MediaStream>(info.Holder())->stream_->GetAudioTracks();
  auto isolate = v8::Isolate::GetCurrent();
  auto tracks = v8::Array::New(isolate, nativeTracks.size());
  size_t i = 0;

  for (auto const & track: nativeTracks) {
    tracks->Set(i, MediaStreamTrack::ToWrapped(track));
    i++;
  }

  info.GetReturnValue().Set(tracks);
}

void MediaStream::GetVideoTracks(v8::Local<v8::String> property, PropertyInfo info) {
  auto nativeTracks = ObjectWrap::Unwrap<MediaStream>(info.Holder())->stream_->GetVideoTracks();
  auto isolate = v8::Isolate::GetCurrent();
  auto tracks = v8::Array::New(isolate, nativeTracks.size());
  size_t i = 0;

  for (auto const & track: nativeTracks) {
    tracks->Set(i, MediaStreamTrack::ToWrapped(track));
    i++;
  }

  info.GetReturnValue().Set(tracks);
}
