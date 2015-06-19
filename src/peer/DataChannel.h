#ifndef SYNTH_BRIDGE_DATA_CHANNEL_H_
#define SYNTH_BRIDGE_DATA_CHANNEL_H_
#pragma once

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>
#include <string>
#include "talk/app/webrtc/datachannelinterface.h"

#include "common/V8.h"
#include "common/EventLoop.h"

class DataChannel
  : public webrtc::DataChannelObserver,
    public node::ObjectWrap {

 public:
  // ObjectWrap boilerplate
  static v8::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object> exports);
  // static void NewInstance(FunctionArgs args);
  static void New(FunctionArgs args);

  static v8::Local<v8::Object> ToWrapped(webrtc::DataChannelInterface* channel);


  //
  // Implements DataChannelObserver
  //
  virtual void OnStateChange();
  virtual void OnMessage(const webrtc::DataBuffer& buffer);

 private:
  explicit DataChannel(webrtc::DataChannelInterface* channel);
  virtual ~DataChannel();

  void EmitEvent(const char* type, PersistentFunction callback);
  void EmitEvent(const char* type, PersistentFunction callback, std::map<const char*, v8::Handle<v8::Value>> params);

  // JS-callable API
  static void Close(FunctionArgs args);
  static void Send(FunctionArgs args);
  static void GetLabel(v8::Local<v8::String> property, PropertyInfo info);

  static void GetEventCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetEventCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
  PersistentFunction onOpen_;
  PersistentFunction onClose_;
  PersistentFunction onError_;
  PersistentFunction onMessage_;


  EventLoop eventLoop_ {"DataChannel"};
  webrtc::DataChannelInterface* channel_;
};

#endif    // SYNTH_BRIDGE_DATA_CHANNEL_H_
