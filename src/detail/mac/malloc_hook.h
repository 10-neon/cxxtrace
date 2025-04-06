#pragma once
#include <atomic>
#include <mutex>
#include <memory>
#include <malloc/malloc.h>

namespace cxxtrace {

    class MallocHook {
    public:
        enum class Status {
            kUninstalled = 0,
            kDisabled = 1,
            kEnabled = 2,
        };
        class Interceptor {
        public:
            virtual ~Interceptor() = default;
            virtual void onAlloc(void* p, std::size_t size) = 0;
            virtual void onDealloc(void* p, std::size_t size) = 0;
        };

        static void install();
        static Status status();
        static void enable();
        static void disable();
        static void setInterceptor(std::shared_ptr<Interceptor> interceptor);

        static std::size_t allocated();
        static std::size_t deallocated();
    private:
        static void *malloc_hook(malloc_zone_t *zone, size_t size);
        static void *calloc_hook(malloc_zone_t *zone, size_t num_items, size_t size);
        static void *realloc_hook(malloc_zone_t *zone, void *ptr, size_t size);
        static void free_hook(malloc_zone_t *zone, void *ptr);
        static void try_free_default_hook(malloc_zone_t *zone, void *ptr);
        static void free_definite_size_hook(malloc_zone_t *zone, void *ptr, size_t size);
        static void on_alloc(void* ptr, size_t size);
        static void on_dealloc(void* ptr, size_t size);
        static malloc_zone_t *get_default_zone();

        // static Interceptor interceptor_;
        static std::mutex mutex_;
        static std::shared_ptr<Interceptor> interceptor_;
        static std::once_flag installed_flag_;
        static malloc_zone_t original_malloc_zone_;
        static std::atomic<Status> status_;
        static thread_local bool inside_disable_guard_;
        static std::atomic<std::size_t> allocated_;
        static std::atomic<std::size_t> deallocated_;
    };

} // cxxtrace
