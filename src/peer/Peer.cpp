#include <string>
#include <vector>
#include "webrtc/base/common.h"
#include "peer/Peer.h"
#include "common/Logging.h"


v8::Persistent<v8::Function> Peer::constructor;
static const char kDefaultStunServer[] = "stun:stun.l.google.com:19302";

Peer::Peer()
  : negotiator_(new Negotiator(this)),
    pcWorkerThread_(new rtc::Thread()),
    pcSignalingThread_(new rtc::Thread()) {

  assert(NULL != pcSignalingThread_ && NULL != pcWorkerThread_);

  pcWorkerThread_->Start();
  pcSignalingThread_->Start();

  pcFactory_ = webrtc::CreatePeerConnectionFactory(
                pcWorkerThread_, pcSignalingThread_, nullptr, nullptr, nullptr);

  if (!pcFactory_.get()) {
    ERROR("Could not create PeerConnectionFactory");
    // @todo some crazy shit
    return;
  }

  eventLoop_.CallAsync([this](void *data) {
    INFO("EVENT_INIT");

    if (!CreatePeerConnection()) {
      // @todo something?
      return;
    }

    negotiator_->CreateOffer();
  });
}

Peer::~Peer() {
  eventLoop_.Terminate();
  negotiator_ = nullptr;
  pc_ = nullptr;
  pcFactory_ = nullptr;

  pcWorkerThread_->Stop();
  pcWorkerThread_->Quit();
  pcSignalingThread_->Stop();
  pcSignalingThread_->Quit();
  delete pcWorkerThread_;
  delete pcSignalingThread_;
}

void Peer::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  // Prepare constructor template
  //   * wrap up our C++ New() method so that JS can call it
  //   * define the "class" name of the object that will be exposed to JS
  //   * our constructor template has one internal field:
  //        a pointer to the real constructor
  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Peer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "onSignal", BindToSignals);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "enumerateDevices", EnumerateDevices);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteAnswer", AddRemoteAnswer);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteCandidate", AddRemoteCandidate);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "Peer"), tpl->GetFunction());
}


void Peer::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Peer(...)`
    Peer* obj = new Peer();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `Peer(...)`, turn into construct call.
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = { };
    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void Peer::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  const unsigned argc = 0;
  v8::Handle<v8::Value> argv[argc] = { };
  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

bool Peer::CreatePeerConnection() {
  ASSERT(nullptr != pcFactory_.get());
  ASSERT(nullptr == pc_.get());

  webrtc::PeerConnectionInterface::RTCConfiguration config;

  // @fixme disabled this for now, it causes it to segfault (11)
  //        actually this is probably the same problem as I'm seeing
  //        with the constriants...
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = kDefaultStunServer;
  config.servers.push_back(server);

  // DTLS-SRTP is the preferred encryption method. If set to kValueFalse, the
  // peer connection uses SDES. Disabling SDES as well will cause the peer
  // connection to fail to connect.
  //
  // Be aware that Web Browsers only implement DTLS, so you'll need to use that
  // if you want to interopt with them.
  //
  webrtc::FakeConstraints constraints;

  constraints.AddMandatory(
             webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
             webrtc::MediaConstraintsInterface::kValueTrue);

  constraints.AddMandatory(
            webrtc::MediaConstraintsInterface::kEnableRtpDataChannels,
            webrtc::MediaConstraintsInterface::kValueTrue);


  // // Things get explodey without these constraints. I'm not sure why yet
  constraints.AddMandatory(
            webrtc::MediaConstraintsInterface::kOfferToReceiveAudio,
            webrtc::MediaConstraintsInterface::kValueFalse);

  constraints.AddMandatory(
            webrtc::MediaConstraintsInterface::kOfferToReceiveVideo,
            webrtc::MediaConstraintsInterface::kValueFalse);

  pc_ = pcFactory_->CreatePeerConnection(config,
                       &constraints,
                       nullptr,            // allocator_factory
                       nullptr,            // IdentityService
                       this);

  if (pc_.get()) {
    negotiator_->SetPC(pc_);
  }

  return nullptr != pc_.get();
}

void Peer::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<Peer>(args.Holder());

  if (nullptr != self->pc_.get()) {
    self->pc_->Close();
  }

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Peer::BindToSignals(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<Peer>(args.Holder());

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  self->eventHandler_ = PersistentFunction::Persistent(isolate,
                                            v8::Handle<v8::Function>::Cast(args[0]));

  args.GetReturnValue().Set(args.This());
}

void Peer::AddRemoteAnswer(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<Peer>(args.Holder());
  auto answer = v8::Handle<v8::Object>::Cast(args[0]);
  v8::String::Utf8Value _sdp(answer->Get(v8::String::NewFromUtf8(isolate, "sdp"))->ToString());
  std::string sdp = *_sdp;

  self->eventLoop_.CallAsync([&self, &sdp](void* data) {
    self->OnSessionDesc("answer", sdp);
  });

  args.GetReturnValue().Set(args.This());
}

void Peer::AddRemoteCandidate(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  auto self = ObjectWrap::Unwrap<Peer>(args.Holder());
  auto candidate = ObjectWrap::Unwrap<IceCandidate>(args[0]->ToObject());

  // Handle<Object> candidateWrapper = Handle<Object>::Cast(args[0]);
  // int sdpMLineIndex = candidateWrapper->Get(String::NewFromUtf8(isolate, "sdpMLineIndex"))->Uint32Value();
  // v8::String::Utf8Value _candidate(candidateWrapper->Get(String::NewFromUtf8(isolate, "candidate"))->ToString());
  // v8::String::Utf8Value _sdpMid(candidateWrapper->Get(String::NewFromUtf8(isolate, "sdpMid"))->ToString());
  // std::string sdpMid = *_sdpMid;
  // std::string candidate = *_candidate;

  self->eventLoop_.CallAsync([&self, &candidate](void* data) {
    INFO("EVENT_HAS_REMOTE_CANDIDATE");
    self->negotiator_->AddIceCandidate(
                        candidate->GetSdpMid(),
                        candidate->GetSdpMLineIndex(),
                        candidate->GetCandidate());
  });

  args.GetReturnValue().Set(args.This());
}


// void Peer::ProcessEvents(uv_async_t* handle, int status) {
//   INFO("PEER::RUN");
//   auto isolate = v8::Isolate::GetCurrent();
//   v8::HandleScope scope(isolate);

//   Peer* self = static_cast<Peer*>(handle->data);
//   bool doShutdown = false;

//   while (true) {
//     uv_mutex_lock(&self->lock_);

//     if (self->events_.empty()) {
//       uv_mutex_unlock(&self->lock_);
//       break;
//     }

//     AsyncEvent event = self->events_.front();
//     self->events_.pop();
//     uv_mutex_unlock(&self->lock_);

//     switch (event.type) {
//       case EVENT_INIT:
//         INFO("EVENT_INIT");
//         if (!self->CreatePeerConnection()) {
//           // @todo something?
//           return;
//         }

//         self->negotiator_->CreateOffer();
//         break;

//       case EVENT_SIGNALING_STATE_CHANGE:
//         {
//           auto data = static_cast<Peer::StateEvent*>(event.data);

//           INFO(("EVENT_SIGNALING_STATE_CHANGE " +
//                       std::to_string(data->state)).c_str());

//           if (webrtc::PeerConnectionInterface::kClosed == data->state) {
//             doShutdown = true;
//           }
//         }
//         break;

//       case EVENT_HAS_REMOTE_CANDIDATE:
//         INFO("EVENT_HAS_REMOTE_CANDIDATE");
//         {
//           auto data = static_cast<Peer::CandidateEvent*>(event.data);
//           self->negotiator_->AddIceCandidate(data->mid, data->mLineIndex, data->sdp);
//         }
//         break;

//       case EVENT_HAS_LOCAL_CANDIDATE:
//         INFO("EVENT_HAS_LOCAL_CANDIDATE");
//         {
//           // auto data = static_cast<Peer::CandidateEvent*>(event.data);

//           // @TODO

//           // auto isolate = v8::Isolate::GetCurrent();

//           // Local<Object> eventObj = Object::New(isolate);
//           // eventObj->Set(String::NewFromUtf8(isolate, "type"),
//           //   String::NewFromUtf8(isolate, "offer", String::kNormalString, "offer".length()));

//           // obj->Set(String::NewFromUtf8(isolate, "sdp"),
//           //   String::NewFromUtf8(isolate, sdp.c_str(), String::kNormalString, sdp.length()));

//           self->EmitEvent("candidate", "sdp");
//         }
//         break;

//       case EVENT_HAS_SESSION_DESC:
//         INFO("EVENT_HAS_SESSION_DESC");
//         {
//           auto data = static_cast<Peer::SdpEvent*>(event.data);

//           if (data->type == "offer") {
//             self->EmitEvent("offer", data->sdp);
//           } else if (data->type == "answer") {
//             self->negotiator_->AddRemoteAnswer(data->sdp);
//           }
//         }

//         break;

//       case EVENT_ENUMERATE_DEVICES:
//         // INFO("EVENT_ENUMERATE_DEVICES");
//         // {
//         //   auto data = static_cast<Peer::EnumerateDevicesEvent*>(event.data);

//         //   std::vector<cricket::Device> audioDevices;
//         //   std::vector<cricket::Device> videoDevices;

//         //   if (data->audio) {
//         //     INFO("Enumerating Audio devices");
//         //     self->deviceManager_->GetAudioInputDevices(&audioDevices);
//         //   }

//         //   if (data->video) {
//         //     INFO("Enumerating Video devices");
//         //     self->deviceManager_->GetVideoCaptureDevices(&videoDevices);
//         //   }

//         //   // build js friendly object
//         //   //
//         //   auto isolate = v8::Isolate::GetCurrent();
//         //   {
//         //     const unsigned argc = 2;
//         //     // v8::HandleScope scope(isolate);

//         //     size_t deviceIndex = 0;
//         //     Local<Array> devices = v8::Array::New(isolate,
//         //                     audioDevices.size() + videoDevices.size());

//         //     for (auto const & device: audioDevices) {
//         //       INFO(("Audio Device: " + device.name).c_str());
//         //       devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("audioinput", device));
//         //       deviceIndex++;
//         //     }

//         //     for (auto const & device: videoDevices) {
//         //       INFO(("Video Device: " + device.name).c_str());
//         //       devices->Set(deviceIndex, MediaDeviceInfo::ToWrapped("videoinput", device));
//         //       deviceIndex++;
//         //     }

//         //     Local<Value> argv[argc] = { v8::Undefined(isolate), devices };
//         //     V8Helpers::CallFn(data->callback, argc, argv);

//         //     // @todo free up data?
//         //   }
//         }
//         break;

//       default:
//         printf("Unexpected event type: %uz\n", event.type);
//         break;
//     }
//   }

//   if (doShutdown) {
//     INFO("Shutting down event loop");
//     uv_close((uv_handle_t*) &self->async_, NULL);
//   }

//   INFO("EXITING PEER::RUN");
// }

//
// PeerConnectionObserver implementation.
//

void Peer::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState) {
  // auto data = new StateEvent(static_cast<uint32_t>(newState));
  // QueueEvent(Peer::EVENT_SIGNALING_STATE_CHANGE, static_cast<void*>(data));

  eventLoop_.CallAsync([this, &newState](void* data) {
    // auto data = static_cast<Peer::StateEvent*>(event.data);

    INFO("EVENT_SIGNALING_STATE_CHANGE %s",
      std::to_string(static_cast<uint32_t>(newState)).c_str());

    if (webrtc::PeerConnectionInterface::kClosed == newState) {
      eventLoop_.Terminate();
    }
  });
}


void Peer::OnAddStream(webrtc::MediaStreamInterface* stream) {
  // push into event queue
}

void Peer::OnRemoveStream(webrtc::MediaStreamInterface* stream) {
  // push into event queue
}

void Peer::OnDataChannel(webrtc::DataChannelInterface* channel) {
  // push into event queue
}

void Peer::OnRenegotiationNeeded() {
  // push into event queue
}

void Peer::OnIceChange() {
  // push into event queue
}

void Peer::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  // auto data = new CandidateEvent(candidate->sdp_mid(),
  //                                 candidate->sdpMLineIndex(),
  //                                 candidate->candidate().ToString());

  eventLoop_.CallAsync([this, &candidate](void* data) {
    INFO("EVENT_HAS_LOCAL_CANDIDATE");
    // auto data = static_cast<Peer::CandidateEvent*>(event.data);

    // @TODO

    // auto isolate = v8::Isolate::GetCurrent();

    // Local<Object> eventObj = Object::New(isolate);
    // eventObj->Set(String::NewFromUtf8(isolate, "type"),
    //   String::NewFromUtf8(isolate, "offer", String::kNormalString, "offer".length()));

    // obj->Set(String::NewFromUtf8(isolate, "sdp"),
    //   String::NewFromUtf8(isolate, sdp.c_str(), String::kNormalString, sdp.length()));

    EmitEvent("candidate", "sdp");
  });
}

void Peer::OnSessionDesc(std::string type, std::string sdp) {
  INFO("EVENT_HAS_SESSION_DESC");

  if (type == "offer") {
    EmitEvent("offer", sdp);
  } else if (type == "answer") {
    negotiator_->AddRemoteAnswer(sdp);
  } else {
    ERROR((std::string("Got an unexpected session description type: ") +
              type).c_str());
  }
}

//
// NegotiationHandlerInterface implementation
//
void Peer::OnLocalOffer(webrtc::SessionDescriptionInterface* desc) {
  std::string type = desc->type();
  std::string sdp;
  desc->ToString(&sdp);

  INFO("OnLocalOffer: %s", sdp.c_str());

  eventLoop_.CallAsync([this, &type, &sdp](void* data) {
    OnSessionDesc(type, sdp);
  });
}


void Peer::EmitEvent(const std::string &type, const std::string &sdp) {
  INFO("EmitEvent: %s", (type + "\n" + sdp).c_str());

  auto isolate = v8::Isolate::GetCurrent();
  const unsigned argc = 1;

  auto obj = v8::Object::New(isolate);
  obj->Set(v8::String::NewFromUtf8(isolate, "type"), V8Helpers::CoerceToV8Str(sdp));
  obj->Set(v8::String::NewFromUtf8(isolate, "sdp"), V8Helpers::CoerceToV8Str(sdp));
  v8::Local<v8::Value> argv[argc] = { obj };

  V8Helpers::CallFn(eventHandler_, argc, argv);
}
