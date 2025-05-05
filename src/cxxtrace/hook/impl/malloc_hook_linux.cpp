#include <malloc.h>

#include <atomic>
#include <iostream>
#include <mutex>

#include "malloc_hook.h"
#include "malloc_hook_disable_guard.h"
#include "plthook.h"

namespace neon {
static decltype(malloc)* origin_malloc{nullptr};
static decltype(free)* origin_free{nullptr};
static decltype(calloc)* origin_calloc{nullptr};
static decltype(realloc)* origin_realloc{nullptr};

static void* malloc_wrap(std::size_t size) {
    void* ret = (*origin_malloc)(size);
    MallocInterposition::onAlloc(malloc_usable_size(ret));
    return ret;
}

static void free_wrap(void* p) {
    MallocInterposition::onDealloc(malloc_usable_size(p));
    (*origin_free)(p);
}

static void* calloc_wrap(std::size_t n, std::size_t sz) {
    void* ret = (*origin_calloc)(n, sz);
    MallocInterposition::onAlloc(malloc_usable_size(ret));
    return ret;
}

static void* realloc_wrap(void* p, std::size_t sz) {
    std::size_t old_size = malloc_usable_size(p);
    void* ret = (*origin_realloc)(p, sz);
    std::size_t new_size = malloc_usable_size(p);
    std::size_t allocated_bytes{0}, deallocated_bytes{0};
    if (ret == p) {
        if (new_size > old_size) {
            allocated_bytes = new_size - old_size;
        } else {
            deallocated_bytes = old_size - new_size;
        }
    } else {
        allocated_bytes = new_size;
        deallocated_bytes = old_size;
    }

    MallocInterposition::onAlloc(allocated_bytes);
    MallocInterposition::onDealloc(deallocated_bytes);
    return ret;
}

bool MallocInterposition::install() {
    MallocHookDisableGuard::install();
    std::call_once(s_install_once, []() {
        // ensure executable file has calloc、realloc
        auto p = calloc(1, 1);
        p = realloc(p, 1);
        free(p);

        MallocHookDisableGuard guard;
        plthook_t* plthook{nullptr};
        if (plthook_open(&plthook, nullptr)) {
            return false;
        }
        if (plthook_replace(plthook, "malloc", (void*)malloc_wrap,
                            (void**)&origin_malloc)) {
            return false;
        }
        if (plthook_replace(plthook, "free", (void*)free_wrap,
                            (void**)&origin_free)) {
            return false;
        }
        if (plthook_replace(plthook, "calloc", (void*)calloc_wrap,
                            (void**)&origin_calloc)) {
            return false;
        }
        if (plthook_replace(plthook, "realloc", (void*)realloc_wrap,
                            (void**)&origin_realloc)) {
            return false;
        }
        if (plthook) {
            plthook_close(plthook);
        }
    });
    return true;
}
}  // namespace neon
