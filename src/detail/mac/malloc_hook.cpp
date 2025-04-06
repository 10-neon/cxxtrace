#include "malloc_hook.h"
#include <sys/mman.h>

namespace cxxtrace {
    std::atomic<MallocHook::Status> MallocHook::status_{MallocHook::Status::kUninstalled};
    std::mutex MallocHook::mutex_{};
    std::shared_ptr<MallocHook::Interceptor> MallocHook::interceptor_{nullptr};
    std::once_flag MallocHook::installed_flag_{};
    malloc_zone_t MallocHook::original_malloc_zone_{};
    thread_local bool MallocHook::inside_disable_guard_{false};
    std::atomic<std::size_t> MallocHook::allocated_{0};
    std::atomic<std::size_t> MallocHook::deallocated_{0};

    malloc_zone_t * MallocHook::get_default_zone() {
        malloc_zone_t **zones = nullptr;
        unsigned int num_zones = 0;
        if (KERN_SUCCESS != malloc_get_all_zones(0, nullptr, (vm_address_t**) &zones,
                                                 &num_zones)) { num_zones = 0; }
        if (num_zones)
            return zones[0];

        return malloc_default_zone();
    }

    void MallocHook::on_alloc(void* ptr, size_t size) {
        allocated_ += size;
        if (inside_disable_guard_) {
            return;
        }
        inside_disable_guard_ = true;
        if (status_ == Status::kEnabled && interceptor_) {
            interceptor_->onAlloc(ptr, size);
        }
        inside_disable_guard_ = false;
    }
    void MallocHook::on_dealloc(void* ptr, size_t size) {
        deallocated_ += size;
        if (inside_disable_guard_) {
            return;
        }
        inside_disable_guard_ = true;
        if (status_ == Status::kEnabled && interceptor_) {
            interceptor_->onDealloc(ptr, size);
        }
        inside_disable_guard_ = false;
    }
    void *MallocHook::malloc_hook(malloc_zone_t *zone, size_t size) {

        void *ret = original_malloc_zone_.malloc(zone, size);

        on_alloc(ret, size);
        return ret;
    }

    void *MallocHook::calloc_hook(malloc_zone_t *zone, size_t num_items, size_t size) {
        void *ret = original_malloc_zone_.calloc(zone, num_items, size);
        on_alloc(ret, size);
        return ret;
    }

    void *MallocHook::realloc_hook(malloc_zone_t *zone, void *ptr, size_t size) {
        void* ret = original_malloc_zone_.realloc(zone, ptr, size);
        on_alloc(ret, size);
        return ret;
    }

    void MallocHook::free_hook(malloc_zone_t *zone, void *ptr) {
        on_dealloc(ptr, malloc_size(ptr));
        original_malloc_zone_.free(zone, ptr);
    }

    void MallocHook::try_free_default_hook(malloc_zone_t *zone, void *ptr) {
        on_dealloc(ptr, malloc_size(ptr));
        original_malloc_zone_.try_free_default(zone, ptr);
    }

    void MallocHook::free_definite_size_hook(malloc_zone_t *zone, void *ptr, size_t size) {
        on_dealloc(ptr, size);
        original_malloc_zone_.free_definite_size(zone, ptr, size);
    }

    std::size_t MallocHook::allocated() {
        return allocated_;
    }
    std::size_t MallocHook::deallocated() {
        return deallocated_;
    }
    void MallocHook::install() {
        if (status_ != MallocHook::Status::kUninstalled) {
            return;
        }
        std::call_once(installed_flag_, []() {
            // thread_local第一次使用时会调用malloc分配内存
            inside_disable_guard_ = false;
            status_ = Status::kDisabled;
            allocated_ = 0;
            deallocated_ = 0;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                interceptor_ = nullptr;
            }
            malloc_zone_t* default_zone = get_default_zone();
            original_malloc_zone_ = *default_zone;
            mprotect(default_zone, sizeof(malloc_zone_t), PROT_READ | PROT_WRITE);
            default_zone->malloc = malloc_hook;
            default_zone->calloc = calloc_hook;
            default_zone->realloc = realloc_hook;
            default_zone->free = free_hook;
            default_zone->try_free_default = try_free_default_hook;
            default_zone->free_definite_size = free_definite_size_hook;
            mprotect(default_zone, sizeof(malloc_zone_t), PROT_READ);
            // 风险比较大
        });
    }

    void MallocHook::enable() {
        status_ = Status::kEnabled;
    }
    void MallocHook::disable() {
        status_ = Status::kDisabled;
    }
    MallocHook::Status MallocHook::status() {
        return status_;
    }
    void MallocHook::setInterceptor(std::shared_ptr<Interceptor> interceptor) {
        std::lock_guard<std::mutex> lock(mutex_);
        interceptor_ = std::move(interceptor);
    }

} // cxxtrace