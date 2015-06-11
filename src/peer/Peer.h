#ifndef SYNTH_BRIDGE_PEER_H_
#define SYNTH_BRIDGE_PEER_H_
#pragma once

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>
#include <string>
#include <queue>
#include <memory>
#include <vector>
#include "webrtc/base/scoped_ptr.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/test/fakeconstraints.h"

#include "common/V8.h"
#include "negotiation/NegotiationHandlerInterface.h"
#include "negotiation/Negotiator.h"
#include "negotiation/IceCandidate.h"
#include "common/EventLoop.h"

// setup PC
// negotiation
// expose data channel messages
//
// Negotiation
//  - assume the server offers
//  - it should offer as so as there is a WS connection
//
class Peer
  : public webrtc::PeerConnectionObserver,
    public NegotiationHandlerInterface,
    public node::ObjectWrap {

 public:
  static v8::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void BindToSignals(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddRemoteAnswer(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddRemoteCandidate(const v8::FunctionCallbackInfo<v8::Value>& args);

  //
  // PeerConnectionObserver implementation.
  //
  virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState);
  virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
  virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
  virtual void OnDataChannel(webrtc::DataChannelInterface* channel);
  virtual void OnRenegotiationNeeded();
  virtual void OnIceChange();
  virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

  //
  // NegotiationHandlerInterface implementation
  //
  virtual void OnLocalOffer(webrtc::SessionDescriptionInterface* desc);

  // enum AsyncEventType {
  //   EVENT_INIT,
  //   EVENT_SIGNALING_STATE_CHANGE,
  //   EVENT_HAS_REMOTE_CANDIDATE,
  //   EVENT_HAS_LOCAL_CANDIDATE,
  //   EVENT_HAS_SESSION_DESC,
  //   EVENT_ENUMERATE_DEVICES
  // };

  // struct StateEvent {
  //   explicit StateEvent(uint32_t state) : state(state) {}

  //   uint32_t state;
  // };

  // struct SdpEvent {
  //   SdpEvent(const std::string type, const std::string sdp)
  //     : type(type), sdp(sdp) {}

  //   const std::string type;
  //   const std::string sdp;
  // };

  // struct CandidateEvent {
  //   CandidateEvent(const std::string mid, int mLineIndex, const std::string &sdp)
  //     : mid(mid), mLineIndex(mLineIndex), sdp(sdp) {}

  //   const std::string mid;
  //   int mLineIndex;
  //   const std::string sdp;
  // };

  // struct EnumerateDevicesEvent {
  //   EnumerateDevicesEvent(bool video, bool audio, PersistentFunction callback)
  //     : video(video), audio(audio), callback(callback) {}

  //   bool video;
  //   bool audio;
  //   PersistentFunction callback;
  // };


 private:
  explicit Peer();
  virtual ~Peer();

  // static void ProcessEvents(uv_async_t* handle, int status);

  void QueueEvent(AsyncEventType type, void* data);
  void EmitEvent(const std::string &type, const std::string &sdp);
  void OnSessionDesc(std::string type, std::string sdp);
  bool CreatePeerConnection();

 private:
  struct AsyncEvent {
    AsyncEventType type;
    void* data;
  };

  EventLoop eventLoop_;
  PersistentFunction eventHandler_;

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcFactory_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;

  std::unique_ptr<Negotiator> negotiator_;

  rtc::Thread *pcWorkerThread_;
  rtc::Thread *pcSignalingThread_;
};

#endif  // SYNTH_BRIDGE_PEER_H_
