#ifndef SYNTH_BRIDGE_MEDIA_DEVICES_WRAPPER_H_
#define SYNTH_BRIDGE_MEDIA_DEVICES_WRAPPER_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <vector>
#include "devices/MediaDevices.h"
#include "common/EventLoop.h"
#include "common/V8.h"

// Designed to expose something like the JS WebRTC MediaDevices interface
// to Node.
//
// This wraps the Native MediaDevices implementation up for now.
//
// interface MediaDevices : EventTarget {
//                 attribute EventHandler ondevicechange;
//     Promise<sequence<MediaDeviceInfo>> enumerateDevices ();
// };
//
class MediaDevicesWrapper : public node::ObjectWrap {
 public:
  explicit MediaDevicesWrapper();
  ~MediaDevicesWrapper();

  static void Init(v8::Handle<v8::Object> exports);
  static void New(FunctionArgs args);
  static void NewInstance(FunctionArgs args);

 private:
  static v8::Persistent<v8::Function> constructor;

  struct EnumerateDevicesEvent {
    EnumerateDevicesEvent(bool video, bool audio, PersistentFunction callback)
      : video(video), audio(audio), callback(callback) {}

    bool video;
    bool audio;
    PersistentFunction callback;
  };

  static void GetOnDeviceChange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetOnDeviceChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
  static void EnumerateDevices(FunctionArgs args);
  void OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback);
  static void GetMedia(FunctionArgs args);
  void OnGetMedia(MediaConstraints constraints, PersistentFunction callback);

  v8::Local<v8::Array> WrapNativeDevices(DeviceCollection audio, DeviceCollection video);

  EventLoop eventLoop_ {"MediaDevices"};
  PersistentFunction onDeviceChange_;
  MediaDevices mediaDevices_;
};

#endif    // SYNTH_BRIDGE_MEDIA_DEVICES_H_
