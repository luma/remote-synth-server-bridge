#ifndef SYNTH_BRIDGE_MEDIA_DEVICE_INFO_H_
#define SYNTH_BRIDGE_MEDIA_DEVICE_INFO_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include "talk/media/base/device.h"

class MediaDeviceInfo : public node::ObjectWrap {
 public:
  MediaDeviceInfo(std::string id, std::string kind,
    std::string label, std::string groupId);

  ~MediaDeviceInfo() {}

  static void Init(v8::Handle<v8::Object> exports);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

  static v8::Local<v8::Object> ToWrapped(std::string kind, const cricket::Device &device);

 private:
  static v8::Persistent<v8::Function> constructor;

  static void GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  static void GetKind(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  static void GetLabel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  static void GetGroupId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

  std::string id_;
  std::string kind_;
  std::string label_;
  std::string groupId_;
};

#endif    // SYNTH_BRIDGE_MEDIA_DEVICE_INFO_H_
