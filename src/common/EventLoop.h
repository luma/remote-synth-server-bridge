#ifndef SYNTH_BRIDGE_EVENT_LOOP_H_
#define SYNTH_BRIDGE_EVENT_LOOP_H_
#pragma once

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>
#include <uv.h>
#include <string>
#include <queue>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include "common/Guid.h"

// namespace SynthBridge {

typedef std::function<void (void* data)> Callback;
struct CallbackEntry {
  Guid id;
  Callback fn;
};
typedef std::vector<CallbackEntry> Callbacks;
typedef std::string AsyncEventType;


class AsyncEvent {
 public:
  explicit AsyncEvent() : data(nullptr) {}

  // explicit AsyncEvent(AsyncEvent& event)
  //   : type(event.type), data(event.data) {}

  AsyncEvent(AsyncEventType type, void* data)
    : type(type), data(data) {}

  AsyncEvent& operator=(AsyncEvent rhs) {
    rhs.swap(*this);
    return *this;
  }

  AsyncEventType type;
  void* data;

  // Also see non-throwing swap idiom
  void swap(AsyncEvent &a) throw () {
     std::swap(this->type, a.type);
     std::swap(this->data, a.data);
  }
};

class EventLoop {
 public:
  EventLoop(const char* name);
  ~EventLoop();

  void Terminate();
  bool isTerminating() const { return terminating_; }
  bool isRunning() const { return !terminating_; }

  void Queue(AsyncEventType type, void* data = nullptr);
  void Queue(const char* type, void* data) {
    Queue(std::string(type), data);
  }

  Guid Bind(AsyncEventType type, Callback callback) {
    return Bind(NewGuid(), type, callback);
  }
  auto Bind(const char* type, Callback callback) {
    return Bind(std::string(type), callback);
  }

  void Unbind(AsyncEventType type, Guid id);

  Guid Once(AsyncEventType type, Callback callback);
  auto Once(const char* type, Callback callback) {
    return Once(std::string(type), callback);
  }

  void CallAsync(Callback callback, void* data = nullptr);

  // @todo Unbind

 private:
  static void ProcessEvents(uv_async_t* handle, int status);
  Guid Bind(Guid id, AsyncEventType type, Callback callback);

  bool terminating_;
  uv_mutex_t lock_;
  uv_async_t async_;
  uv_loop_t *loop_;

  const char *name_;
  std::queue<AsyncEvent> events_;
  std::map<std::string, Callbacks> callbackMap_;
};

// }

#endif    // SYNTH_BRIDGE_LOGGING_H_
