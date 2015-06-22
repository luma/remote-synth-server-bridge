#ifndef SYNTH_BRIDGE_MEDIA_STREAM_H_
#define SYNTH_BRIDGE_MEDIA_STREAM_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include "webrtc/base/scoped_ref_ptr.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "common/V8.h"

class MediaStream : public node::ObjectWrap {
 public:
  explicit MediaStream(webrtc::MediaStreamInterface* stream);
  ~MediaStream();

  static void Init(v8::Handle<v8::Object> exports);
  static void New(FunctionArgs args);

  static v8::Local<v8::Object> ToWrapped(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

  static void AddTrack(FunctionArgs args);
  static void RemoveTrack(FunctionArgs args);
  static void FindAudioTrack(FunctionArgs args);
  static void FindVideoTrack(FunctionArgs args);
  static void GetLabel(v8::Local<v8::String> property, PropertyInfo info);
  static void GetAudioTracks(v8::Local<v8::String> property, PropertyInfo info);
  static void GetVideoTracks(v8::Local<v8::String> property, PropertyInfo info);

  webrtc::MediaStreamInterface* Value() {
    return stream_.get();
  }

 private:
  static v8::Persistent<v8::Function> constructor;

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream_;
};

#endif    // SYNTH_BRIDGE_MEDIA_STREAM_H_
