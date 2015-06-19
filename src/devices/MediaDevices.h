#ifndef SYNTH_BRIDGE_MEDIA_DEVICES_H_
#define SYNTH_BRIDGE_MEDIA_DEVICES_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <vector>
#include "talk/media/devices/devicemanager.h"
#include "devices/MediaDeviceInfo.h"
#include "common/EventLoop.h"
#include "common/V8.h"

// interface MediaDevices : EventTarget {
//                 attribute EventHandler ondevicechange;
//     Promise<sequence<MediaDeviceInfo>> enumerateDevices ();
// };
//
class MediaDevices : public node::ObjectWrap {
 public:
  explicit MediaDevices();
  ~MediaDevices();

  static void Init(v8::Handle<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

  static v8::Local<v8::Object> ToWrapped(std::string kind, const cricket::Device &device);

 private:
  static v8::Persistent<v8::Function> constructor;

  struct EnumerateDevicesEvent {
    EnumerateDevicesEvent(bool video, bool audio, PersistentFunction callback)
      : video(video), audio(audio), callback(callback) {}

    bool video;
    bool audio;
    PersistentFunction callback;
  };

  static void GetId(v8::Local<v8::String> property,
                  PropertyInfo info);

  static void GetOnDeviceChange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetOnDeviceChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
  static void EnumerateDevices(const v8::FunctionCallbackInfo<v8::Value>& args);
  void OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback);

  EventLoop eventLoop_ {"MediaDevices"};
  PersistentFunction onDeviceChange_;
  rtc::scoped_ptr<cricket::DeviceManagerInterface> deviceManager_;
};

#endif    // SYNTH_BRIDGE_MEDIA_DEVICES_H_
