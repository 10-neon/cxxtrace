#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>

#include "malloc_hook_disable_guard.h"

namespace neon {

class MallocListener {
   public:
    virtual void alloc(std::size_t bytes) = 0;
    virtual void dealloc(std::size_t bytes) = 0;
};

class MallocInterposition {
   public:
    static bool install();
    static void setListener(MallocListener* listener) { s_listener = listener; }
    static MallocListener* listener() { return s_listener; }
    static void enable() { s_enable = true; }
    static bool isEnable() { return s_enable; }
    static void disable() { s_enable = false; }

    static void onDealloc(std::size_t size) {
        if (MallocInterposition::isEnable() &&
            !MallocHookDisableGuard::isDisable()) {
            if (auto malloc_listener = MallocInterposition::listener()) {
                MallocHookDisableGuard guard;
                malloc_listener->dealloc(size);
            }
        }
    }

    static void onAlloc(std::size_t size) {
        if (MallocInterposition::isEnable() &&
            !MallocHookDisableGuard::isDisable()) {
            if (auto malloc_listener = MallocInterposition::listener()) {
                MallocHookDisableGuard guard;
                malloc_listener->alloc(size);
            }
        }
    }

   private:
    static std::once_flag s_install_once;
    static std::atomic<MallocListener*> s_listener;
    static std::atomic<bool> s_enable;
};

}  // namespace neon
