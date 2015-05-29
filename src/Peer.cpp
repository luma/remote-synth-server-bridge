#include <string>
#include "webrtc/base/common.h"
#include "Peer.h"
#include "Common.h"

using namespace v8;

Persistent<Function> Peer::constructor;
static const char kDefaultStunServer[] = "stun:stun.l.google.com:19302";

Peer::Peer()
  : loop_(uv_default_loop()),
    pcFactory_(webrtc::CreatePeerConnectionFactory()),
    negotiator_(new Negotiator(this)) {

  if (!pcFactory_.get()) {
    // @todo some crazy shit
    return;
  }

  uv_mutex_init(&lock_);
  uv_async_init(loop_, &async_, reinterpret_cast<uv_async_cb>(Run));
  async_.data = this;

  QueueEvent(EVENT_INIT, static_cast<void*>(NULL));
}

Peer::~Peer() {
  negotiator_ = nullptr;
  pc_ = nullptr;
  pcFactory_ = nullptr;
}

void Peer::Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Peer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "onSignal", BindToSignals);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteAnswer", AddRemoteAnswer);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteCandidate", AddRemoteCandidate);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Peer"), tpl->GetFunction());
}


void Peer::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Peer(...)`
    Peer* obj = new Peer();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `Peer(...)`, turn into construct call.
    const int argc = 0;
    Local<Value> argv[argc] = { };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void Peer::NewInstance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  const unsigned argc = 0;
  Handle<Value> argv[argc] = { };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void Peer::QueueEvent(AsyncEventType type, void* data) {
  AsyncEvent event = {type, data};

  uv_mutex_lock(&lock_);
  events_.push(event);
  uv_mutex_unlock(&lock_);

  uv_async_send(&async_);
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
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           webrtc::MediaConstraintsInterface::kValueTrue);

  // @fixme things get explodey without these constraints. I'm not sure why yet
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio,
                           webrtc::MediaConstraintsInterface::kValueFalse);
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo,
                           webrtc::MediaConstraintsInterface::kValueFalse);

  pc_ = pcFactory_->CreatePeerConnection(config,
                       &constraints,
                       nullptr,            // allocator_factory
                       nullptr,            // IdentityService
                       this);

  negotiator_->SetPC(pc_);
  return nullptr != pc_.get();
}

void Peer::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Peer* self = ObjectWrap::Unwrap<Peer>(args.Holder());

  if (nullptr != self->pc_.get()) {
    self->pc_->Close();
  }

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Peer::BindToSignals(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Peer* self = ObjectWrap::Unwrap<Peer>(args.Holder());

  // get callback argument. It is a function; cast it to a Function and
  // store the function in a Persistent handle, since we also want that
  // to remain after this call returns
  self->eventHandler_ = PersistentFunction::Persistent(isolate,
                                            Handle<Function>::Cast(args[0]));

  args.GetReturnValue().Set(args.This());
}

void Peer::AddRemoteAnswer(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Peer* self = ObjectWrap::Unwrap<Peer>(args.Holder());
  Handle<Object> answer = Handle<Object>::Cast(args[0]);
  v8::String::Utf8Value _sdp(answer->Get(String::NewFromUtf8(isolate, "sdp"))->ToString());

  SdpEvent* data = new SdpEvent("answer", *_sdp);
  self->QueueEvent(Peer::EVENT_HAS_SESSION_DESC, static_cast<void*>(data));

  args.GetReturnValue().Set(args.This());
}

void Peer::AddRemoteCandidate(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Peer* self = ObjectWrap::Unwrap<Peer>(args.Holder());

  Handle<Object> candidate = Handle<Object>::Cast(args[0]);
  uint32_t sdp_mline_index = candidate->Get(String::NewFromUtf8(isolate, "sdpMLineIndex"))->Uint32Value();
  v8::String::Utf8Value _candidate(candidate->Get(String::NewFromUtf8(isolate, "candidate"))->ToString());
  v8::String::Utf8Value _sipMid(candidate->Get(String::NewFromUtf8(isolate, "sdpMid"))->ToString());

  CandidateEvent* data = new CandidateEvent(*_sipMid, sdp_mline_index, *_candidate);
  self->QueueEvent(Peer::EVENT_HAS_REMOTE_CANDIDATE, static_cast<void*>(data));

  args.GetReturnValue().Set(args.This());
}


void Peer::Run(uv_async_t* handle, int status) {
  INFO("PEER::RUN");
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Peer* self = static_cast<Peer*>(handle->data);
  bool doShutdown = false;

  while (true) {
    uv_mutex_lock(&self->lock_);

    if (self->events_.empty()) {
      uv_mutex_unlock(&self->lock_);
      break;
    }

    AsyncEvent event = self->events_.front();
    self->events_.pop();
    uv_mutex_unlock(&self->lock_);

    switch (event.type) {
      case EVENT_INIT:
        INFO("EVENT_INIT");
        if (!self->CreatePeerConnection()) {
          // @todo something?
          return;
        }

        self->negotiator_->CreateOffer();
        break;

      case EVENT_SIGNALING_STATE_CHANGE:
        INFO("EVENT_SIGNALING_STATE_CHANGE");
        {
          Peer::StateEvent* data = static_cast<Peer::StateEvent*>(event.data);

          if (webrtc::PeerConnectionInterface::kClosed == data->state) {
            doShutdown = true;
          }
        }
        break;

      case EVENT_HAS_REMOTE_CANDIDATE:
        INFO("EVENT_HAS_REMOTE_CANDIDATE");
        {
          Peer::CandidateEvent* data = static_cast<Peer::CandidateEvent*>(event.data);
          self->negotiator_->AddIceCandidate(data->mid, data->mLineIndex, data->sdp);
        }
        break;

      case EVENT_HAS_LOCAL_CANDIDATE:
        INFO("EVENT_HAS_LOCAL_CANDIDATE");
        {
          // Peer::CandidateEvent* data = static_cast<Peer::CandidateEvent*>(event.data);

          // @TODO

          // Isolate* isolate = Isolate::GetCurrent();

          // Local<Object> eventObj = Object::New(isolate);
          // eventObj->Set(String::NewFromUtf8(isolate, "type"),
          //   String::NewFromUtf8(isolate, "offer", String::kNormalString, "offer".length()));

          // obj->Set(String::NewFromUtf8(isolate, "sdp"),
          //   String::NewFromUtf8(isolate, sdp.c_str(), String::kNormalString, sdp.length()));

          self->EmitEvent("candidate", "sdp");
        }
        break;

      case EVENT_HAS_SESSION_DESC:
        INFO("EVENT_HAS_SESSION_DESC");
        {
          Peer::SdpEvent* data = static_cast<Peer::SdpEvent*>(event.data);

          if (data->type == "offer") {
            self->EmitEvent("offer", data->sdp);
          } else if (data->type == "answer") {
            self->negotiator_->AddRemoteAnswer(data->sdp);
          }
        }

        break;

      default:
        printf("Unexpected event type: %uz\n", event.type);
        break;
    }
  }

  if (doShutdown) {
    INFO("Shutting down event loop");
    uv_close((uv_handle_t*) &self->async_, NULL);
  }

  INFO("EXITING PEER::RUN");
}

//
// PeerConnectionObserver implementation.
//

void Peer::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState) {
  StateEvent* data = new StateEvent(static_cast<uint32_t>(newState));
  QueueEvent(Peer::EVENT_SIGNALING_STATE_CHANGE, static_cast<void*>(data));
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
  CandidateEvent* data = new CandidateEvent(candidate->sdp_mid(),
                                            candidate->sdp_mline_index(),
                                            candidate->candidate().ToString());

  QueueEvent(Peer::EVENT_HAS_LOCAL_CANDIDATE, static_cast<void*>(data));
}


//
// NegotiationHandlerInterface implementation
//
void Peer::OnLocalOffer(webrtc::SessionDescriptionInterface* desc) {
  std::string sdp;
  desc->ToString(&sdp);
  SdpEvent* data = new SdpEvent(desc->type(), sdp);
  QueueEvent(Peer::EVENT_HAS_SESSION_DESC, static_cast<void*>(data));
}


void Peer::EmitEvent(const std::string &type, const std::string &sdp) {
  INFO((std::string("EmitEvent: ") + type + "\n" + sdp).c_str());

  Isolate* isolate = Isolate::GetCurrent();
  const unsigned argc = 1;

  Local<Object> obj = Object::New(isolate);
  obj->Set(String::NewFromUtf8(isolate, "type"),
            String::NewFromUtf8(isolate, type.c_str(),
                    String::kNormalString, type.length()));

  obj->Set(String::NewFromUtf8(isolate, "sdp"),
            String::NewFromUtf8(isolate, sdp.c_str(),
                      String::kNormalString, sdp.length()));

  Local<Value> argv[argc] = { obj };
  // callback_.Call(isolate->GetCurrentContext()->Global(), argc, argv);

  // Create a handle scope to keep the temporary object references.
  // v8::HandleScope localHandleScope(isolate);

  // Enter the new execution context before evaluating any code so that
  // all the remaining operations take place there
  // v8::Context::Scope contextScope(isolate->GetCurrentContext());

  // Invoke the process function, giving the global object as 'this'
  // and whatever arguments were passed in
  auto callableFn = v8::Local<v8::Function>::New(isolate, eventHandler_);

  {
    // Set up an exception handler before calling the handler function
    v8::TryCatch tryCatch;

    callableFn->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    if (tryCatch.HasCaught()) {
      // @todo do something
    }
  }
}
