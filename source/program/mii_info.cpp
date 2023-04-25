#include <lib.hpp>
#include <nn.hpp>
#include <cstring>
#include <string>

#include "func_ptrs.hpp"
#include "sead/container/seadBuffer.h"
#include "sead/prim/seadSafeString.hpp"

#define LOG(...)                                                    \
  {                                                                 \
      int length = snprintf(buffer, sizeof(buffer), __VA_ARGS__);   \
      svcOutputDebugString(buffer, length);                         \
  }

/*
This file will have all things related to the
"MiiInfo" structure. This is going to get messy.
*/