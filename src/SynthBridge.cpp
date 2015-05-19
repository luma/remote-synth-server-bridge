#include <node.h>
#include "PeerWrapper.h"

using namespace v8;

void CreatePeer(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  PeerWrapper::NewInstance(args);
}

void InitAll(Handle<Object> exports) {
  PeerWrapper::Init(exports);

  NODE_SET_METHOD(exports, "Peer", CreatePeer);
}

NODE_MODULE(SynthBridge, InitAll)
