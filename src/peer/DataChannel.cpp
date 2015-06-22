#include "peer/DataChannel.h"

v8::Persistent<v8::Function> DataChannel::constructor;

DataChannel::DataChannel(webrtc::DataChannelInterface* channel)
  : channel_(channel) {
    channel_->RegisterObserver(this);
  }

DataChannel::~DataChannel() {
  if (channel_) channel_->UnregisterObserver();
  channel_ = nullptr;

  eventLoop_.Terminate();
}

void DataChannel::Init(v8::Handle<v8::Object> exports) {
  auto isolate = v8::Isolate::GetCurrent();

  auto tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "DataChannel"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "send", Send);
  tpl->InstanceTemplate()->SetAccessor(
          v8::String::NewFromUtf8(isolate, "label"), GetLabel, NULL);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "DataChannel"), tpl->GetFunction());
}


void DataChannel::New(FunctionArgs args) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new DataChannel(...)`
    v8::Local<v8::External> _channel = v8::Local<v8::External>::Cast(args[0]);
    auto channel = static_cast<webrtc::DataChannelInterface*>(_channel->Value());

    DataChannel* obj = new DataChannel(channel);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `DataChannel(...)`, turn into construct call.
    // @todo argument
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = { };
    auto cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

// void DataChannel::NewInstance(FunctionArgs args) {
//   auto isolate = v8::Isolate::GetCurrent();
//   v8::HandleScope scope(isolate);

//   const unsigned argc = 0;
//   v8::Handle<v8::Value> argv[argc] = { };
//   auto cons = v8::Local<v8::Function>::New(isolate, constructor);
//   v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);

//   args.GetReturnValue().Set(instance);
// }

v8::Local<v8::Object> DataChannel::ToWrapped(webrtc::DataChannelInterface* channel) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = {
    v8::External::New(isolate, static_cast<void*>(channel))
  };

  auto cons = v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = cons->NewInstance(argc, argv);
  return scope.Escape(instance);
}

void DataChannel::GetEventCallback(v8::Local<v8::String> property, PropertyInfo info) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);
  auto self = ObjectWrap::Unwrap<DataChannel>(info.Holder());
  auto propName = V8Helpers::CoerceFromV8Str(property);

  PersistentFunction callback;

  if (propName == "onopen") {
    callback = self->onOpen_;
  } else if (propName == "onclose") {
    callback = self->onClose_;
  } else if (propName == "onerror") {
    callback = self->onError_;
  } else if (propName == "onmessage") {
    callback = self->onMessage_;
  } else {
    ERROR("DataChannel::GetEventCallback: Unknown property %s", propName.c_str());
    // @todo throw a JS exception
    return;
  }

  info.GetReturnValue().Set(v8::Local<v8::Function>::New(isolate, callback));
}

void DataChannel::SetEventCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
  auto isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  auto self = ObjectWrap::Unwrap<DataChannel>(info.Holder());
  auto propName = V8Helpers::CoerceFromV8Str(property);
  auto callback = PersistentFunction::Persistent(
                        isolate, v8::Handle<v8::Function>::Cast(value));

  if (propName == "onopen") {
    self->onOpen_ = callback;
  } else if (propName == "onclose") {
    self->onClose_ = callback;
  } else if (propName == "onerror") {
    self->onError_ = callback;
  } else if (propName == "onmessage") {
    self->onMessage_ = callback;
  } else {
    ERROR("DataChannel::SetEventCallback: Unknown property %s", propName.c_str());
    // @todo throw a JS exception
  }
}

void DataChannel::GetLabel(v8::Local<v8::String> property, PropertyInfo info) {
  auto value = ObjectWrap::Unwrap<DataChannel>(info.Holder())->channel_->label();
  info.GetReturnValue().Set(V8Helpers::CoerceToV8Str(value));
}

void DataChannel::Send(FunctionArgs args) {
  // @todo
}

void DataChannel::Close(FunctionArgs args) {
  auto self = ObjectWrap::Unwrap<DataChannel>(args.Holder());
  self->channel_->Close();
}

void DataChannel::OnStateChange() {
  auto state = channel_->state();

  eventLoop_.CallAsync([this, state](void* data) {
    INFO("DataChannel::OnStateChange %s",
              webrtc::DataChannelInterface::DataStateString(state));

    switch (state) {
    case webrtc::DataChannelInterface::kOpen:
      EmitEvent("open", onOpen_);
      break;

    case webrtc::DataChannelInterface::kClosed:
      EmitEvent("close", onClose_);
      break;

    default:
      break;
    }
  });
}

void DataChannel::OnMessage(const webrtc::DataBuffer& buffer) {
  eventLoop_.CallAsync([this, buffer](void* args) {
    auto isolate = v8::Isolate::GetCurrent();
    size_t length = buffer.size();
    auto messageType = v8::String::NewFromUtf8(isolate, buffer.binary ? "binary" : "text");
    char* data = new char[length];
    memcpy(static_cast<void*>(data), static_cast<const void*>(buffer.data.data()), length);
    v8::Handle<v8::Value> v8Data;

    if (buffer.binary) {
      v8Data = v8::ArrayBuffer::New(isolate, data, length);
    } else {
      v8Data = v8::String::NewFromUtf8(isolate, data, v8::String::kNormalString, length);
    }

    EmitEvent("message", onMessage_, {
      {"type", messageType},
      {"data", v8Data}
    });

    delete[] data;
    data = NULL;
  });
}

void DataChannel::EmitEvent(const char* type, PersistentFunction callback) {
  EmitEvent(type, callback, {});
}

void DataChannel::EmitEvent(const char* type, PersistentFunction callback, std::map<const char*, v8::Handle<v8::Value>> params) {
  auto isolate = v8::Isolate::GetCurrent();
  auto obj = v8::Object::New(isolate);

  obj->Set(v8::String::NewFromUtf8(isolate, "type"),
            v8::String::NewFromUtf8(isolate, type));

  for (auto param : params) {
    obj->Set(v8::String::NewFromUtf8(isolate, param.first), param.second);
  }

  const unsigned argc = 1;
  v8::Local<v8::Value> argv[argc] = { obj };

  V8Helpers::CallFn(callback, argc, argv);
}
