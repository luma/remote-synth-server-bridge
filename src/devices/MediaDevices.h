#ifndef SYNTH_BRIDGE_MEDIA_DEVICES_H_
#define SYNTH_BRIDGE_MEDIA_DEVICES_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <vector>
#include "webrtc/base/scoped_ptr.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/media/devices/devicemanager.h"
#include "webrtc/modules/audio_device/audio_device_impl.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "devices/MediaDeviceInfo.h"
#include "common/EventLoop.h"
#include "common/V8.h"

typedef std::vector<cricket::Device> DeviceCollection;

// interface MediaDevices : EventTarget {
//                 attribute EventHandler ondevicechange;
//     Promise<sequence<MediaDeviceInfo>> enumerateDevices ();
// };
//
class MediaDevices : public node::ObjectWrap {
 public:
  explicit MediaDevices();
  ~MediaDevices();

  enum Type {
    VIDEO,
    AUDIO
  };

  static void Init(v8::Handle<v8::Object> exports);
  static void New(FunctionArgs args);
  static void NewInstance(FunctionArgs args);

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

  // static void GetId(v8::Local<v8::String> property,
  //                 PropertyInfo info);

  static void GetOnDeviceChange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetOnDeviceChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
  static void EnumerateDevices(FunctionArgs args);
  void OnEnumerateDevices(bool hasVideo, bool hasAudio, PersistentFunction callback);
  static void GetMedia(FunctionArgs args);
  void OnGetMedia(std::string audioDeviceId, std::string videoDeviceId, PersistentFunction callback);

  v8::Local<v8::Array> WrapNativeDevices(DeviceCollection audio, DeviceCollection video);
  cricket::VideoCapturer* GetCapturerById(Type type, std::string deviceId);
  bool GetCaptureDevices(Type type, DeviceCollection* devices);

  EventLoop eventLoop_ {"MediaDevices"};
  PersistentFunction onDeviceChange_;
  rtc::scoped_ptr<cricket::DeviceManagerInterface> deviceManager_;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audioDevice_;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcFactory_;

  rtc::Thread *pcWorkerThread_;
  rtc::Thread *pcSignalingThread_;
};

#endif    // SYNTH_BRIDGE_MEDIA_DEVICES_H_
