#ifndef SYNTH_BRIDGE_COMMON_H_
#define SYNTH_BRIDGE_COMMON_H_

#include <string>
#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"


#define WARN(msg) fprintf(stdout, "\033[01;33m native:%s \033[00m\n", msg)
#define ERROR(msg) fprintf(stdout, "\033[01;32m native:%s \033[00m\n", msg)
#define INFO(msg) fprintf(stdout, "\033[01;34m native:%s \033[00m\n", msg)

#endif    // SYNTH_BRIDGE_COMMON_H_


