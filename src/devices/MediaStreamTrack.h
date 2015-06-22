#ifndef SYNTH_BRIDGE_MEDIA_STREAM_TRACK_H_
#define SYNTH_BRIDGE_MEDIA_STREAM_TRACK_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include "webrtc/base/scoped_ref_ptr.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "common/V8.h"

class MediaStreamTrack : public node::ObjectWrap {
 public:
  explicit MediaStreamTrack(webrtc::MediaStreamTrackInterface* track);
  ~MediaStreamTrack();

  static void Init(v8::Handle<v8::Object> exports);
  static void New(FunctionArgs args);

  static v8::Local<v8::Object> ToWrapped(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

  static void GetId(v8::Local<v8::String> property, PropertyInfo info);
  static void GetKind(v8::Local<v8::String> property, PropertyInfo info);
  static void GetState(v8::Local<v8::String> property, PropertyInfo info);
  static void GetEnabled(v8::Local<v8::String> property, PropertyInfo info);
  static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

  webrtc::MediaStreamTrackInterface* Value() {
    return track_.get();
  }

  std::string GetKind() {
    return track_->kind();
  }

 private:
  static v8::Persistent<v8::Function> constructor;
  const char* StateToString();

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track_;
};

#endif    // SYNTH_BRIDGE_MEDIA_STREAM_TRACK_H_
