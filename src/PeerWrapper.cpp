#include "PeerWrapper.h"

using namespace v8;

Persistent<Function> PeerWrapper::constructor;

PeerWrapper::PeerWrapper() : peer_(new Peer()) {}

PeerWrapper::~PeerWrapper() {
  negotiation_ = NULL;
  peer_ = NULL;
}

void PeerWrapper::Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Peer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteAnswer", AddRemoteAnswer);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "addRemoteCandidate", AddRemoteCandidate);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Peer"),
               tpl->GetFunction());
}

void PeerWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new PeerWrapper(...)`
    PeerWrapper* obj = new PeerWrapper();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `PeerWrapper(...)`, turn into construct call.
    const int argc = 0;
    Local<Value> argv[argc] = { };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void PeerWrapper::NewInstance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  const unsigned argc = 0;
  Handle<Value> argv[argc] = { };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void PeerWrapper::Connect(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  PeerWrapper* obj = ObjectWrap::Unwrap<PeerWrapper>(args.Holder());

  bool success = obj->peer_->Connect(
            new NegotiationInterfaceWrapper(Local<Function>::Cast(args[0])));

  args.GetReturnValue().Set(Number::New(isolate, success));
}

void PeerWrapper::AddRemoteAnswer(const std::string &sdp) {

}

void PeerWrapper::AddRemoteCandidate(
  const std::string &mid, unsigned short mLineIndex, const std::string &sdp) {

}

