#include "common/EventLoop.h"
#include "common/Logging.h"
#include "common/V8.h"

// Non-throwing swap idiom:
//   http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Non-throwing_swap
//
void swap(AsyncEvent & a1, AsyncEvent & a2) /* throw () */ {
  a1.swap(a2);
}

namespace std {
  template <>
  void swap(AsyncEvent & a1, AsyncEvent & a2) /* throw () */ {
    a1.swap(a2);
  }
}

EventLoop::EventLoop() : loop_(uv_default_loop()) {
  uv_mutex_init(&lock_);
  uv_async_init(loop_, &async_, reinterpret_cast<uv_async_cb>(ProcessEvents));
  async_.data = this;
}

EventLoop::~EventLoop() {
  Terminate();
}

void EventLoop::Terminate() {
  INFO("EventLoop:: Shutting down");
  uv_close((uv_handle_t*)(&async_), NULL);
}

void EventLoop::Queue(AsyncEventType type, void* data) {
  AsyncEvent event = {type, data};

  {
    UVLock uvLock(lock_);
    events_.push(event);
  }

  uv_async_send(&async_);
}

Guid EventLoop::Bind(Guid id, AsyncEventType type, Callback callback) {
  UVLock uvLock(lock_);
  callbackMap_[type].push_back({ id, callback });

  return id;
}

void EventLoop::Unbind(AsyncEventType type, Guid id) {
  UVLock uvLock(lock_);
  auto it = callbackMap_.find(type);
  auto callbacks = it->second;

  if (it != callbackMap_.end()) {
    for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
      if (it->id == id) {
        callbacks.erase(it);
        break;
      }
    }
  }
}

Guid EventLoop::Once(AsyncEventType type, Callback callback) {
  auto id = guidGenerator_.newGuid();

  Bind(id, type, [this, type, id, &callback](void* data) {
    callback(data);
    Unbind(type, id);
  });

  return id;
}

void EventLoop::CallAsync(Callback callback, void* data) {
  auto strGuid = NewGuidStr();
  Once(strGuid, callback);
  Queue(strGuid, data);
}

void EventLoop::ProcessEvents(uv_async_t* handle, int status) {
  INFO("EventLoop::ProcessEvents");

  EventLoop* self = static_cast<EventLoop*>(handle->data);

  while (true) {
    AsyncEvent event;

    {
      UVLock uvLock(self->lock_);

      if (self->events_.empty()) {
        break;
      }

      event = self->events_.front();
      self->events_.pop();
    }

    INFO("EventLoop:: Processing a %s...", event.type.c_str());

    auto it = self->callbackMap_.find(event.type);
    auto callbacks = it->second;

    if (it ==  self->callbackMap_.end()) {
      ERROR("EventLoop:: Unexpected event type: %s", event.type.c_str());
      return;
    } else {
      for (auto const &callback : callbacks) {
        callback.fn(event.data);
      }
    }
  }

  INFO("EventLoop::ProcessEvents");
}

std::string EventLoop::NewGuidStr() {
  auto guid = guidGenerator_.newGuid();
  std::stringstream stream;
  stream << guid;
  return stream.str();
}
