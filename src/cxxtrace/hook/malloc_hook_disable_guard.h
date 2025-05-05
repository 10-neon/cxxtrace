#pragma once
#include <pthread.h>

#include <numeric>

namespace neon {

class MallocHookDisableGuard {
   public:
    MallocHookDisableGuard();
    ~MallocHookDisableGuard();
    static bool isDisable();
    static void install();

   private:
    void* origin_;
};
}  // namespace neon
