#include <malloc/malloc.h>
#include <pthread.h>
#include <sys/mman.h>

#include <atomic>
#include <mutex>
#include <unordered_map>

#include "malloc_hook.h"
#include "malloc_hook_disable_guard.h"

namespace neon {
static malloc_zone_t g_original_malloc_zone{};

static void *malloc_hook(malloc_zone_t *zone, size_t size) {
    void *ret = g_original_malloc_zone.malloc(zone, size);
    MallocInterposition::onAlloc(malloc_size(ret));
    return ret;
}

static void *calloc_hook(malloc_zone_t *zone, size_t num_items, size_t size) {
    void *ret = g_original_malloc_zone.calloc(zone, num_items, size);
    MallocInterposition::onAlloc(malloc_size(ret));
    return ret;
}

static void *realloc_hook(malloc_zone_t *zone, void *ptr, size_t size) {
    void *ret = g_original_malloc_zone.realloc(zone, ptr, size);
    // TODO
    return ret;
}

static void free_hook(malloc_zone_t *zone, void *ptr) {
    MallocInterposition::onDealloc(malloc_size(ptr));
    g_original_malloc_zone.free(zone, ptr);
}

static void try_free_default_hook(malloc_zone_t *zone, void *ptr) {
    MallocInterposition::onDealloc(malloc_size(ptr));
    g_original_malloc_zone.try_free_default(zone, ptr);
}

static void free_definite_size_hook(malloc_zone_t *zone, void *ptr,
                                    size_t size) {
    MallocInterposition::onDealloc(malloc_size(ptr));
    g_original_malloc_zone.free_definite_size(zone, ptr, size);
}

static malloc_zone_t *getDefaultZone() {
    malloc_zone_t **zones = nullptr;
    unsigned int num_zones = 0;
    if (KERN_SUCCESS !=
        malloc_get_all_zones(0, nullptr, (vm_address_t **)&zones, &num_zones)) {
        num_zones = 0;
    }
    if (num_zones)
        return zones[0];

    return malloc_default_zone();
}

bool MallocInterposition::install() {
    MallocHookDisableGuard::install();
    std::call_once(s_install_once, []() {
        malloc_zone_t *default_zone = getDefaultZone();
        g_original_malloc_zone = *default_zone;
        mprotect(default_zone, sizeof(malloc_zone_t), PROT_READ | PROT_WRITE);
        default_zone->malloc = malloc_hook;
        default_zone->calloc = calloc_hook;
        default_zone->realloc = realloc_hook;
        default_zone->free = free_hook;
        default_zone->try_free_default = try_free_default_hook;
        default_zone->free_definite_size = free_definite_size_hook;
        mprotect(default_zone, sizeof(malloc_zone_t), PROT_READ);
    });
    return true;
}

}  // namespace neon
