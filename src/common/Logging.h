#ifndef SYNTH_BRIDGE_LOGGING_H_
#define SYNTH_BRIDGE_LOGGING_H_
#pragma once

#include <string>
#include "webrtc/base/refcount.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/logging.h"

// @todo If somekind of debugging flag
// rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
// rtc::LogMessage::LogTimestamps();
// rtc::LogMessage::LogThreads();
// end if @todo

#define WARN(msg) fprintf(stdout, "\033[01;33m native:%s \033[00m\n", msg)
#define ERROR(msg) fprintf(stdout, "\033[01;32m native:%s \033[00m\n", msg)
#define INFO(msg) fprintf(stdout, "\033[01;34m native:%s \033[00m\n", msg)

#endif    // SYNTH_BRIDGE_LOGGING_H_


