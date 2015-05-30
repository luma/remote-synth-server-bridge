#ifndef SYNTH_BRIDGE_COMMON_V8_H_
#define SYNTH_BRIDGE_COMMON_V8_H_
#pragma once

#include <node.h>

typedef v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> PersistentContext;
typedef v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> PersistentFunction;

#endif    // SYNTH_BRIDGE_COMMON_V8_H_
