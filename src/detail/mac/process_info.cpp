#include "cxxtrace/detail/mac/process_info.h"
#include <libproc.h>
#include <mach/mach.h>
#include <mach/task.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <mach/mach_init.h>

namespace cxxtrace {
namespace detail {

MacProcessInfo::MacProcessInfo() {
    // 获取进程ID
    pid_ = ::getpid();
    
    // 获取进程名称
    char name[1024];
    if (::proc_name(pid_, name, sizeof(name)) > 0) {
        name_ = name;
    }
}

std::string MacProcessInfo::process_name() const {
    return name_;
}

int64_t MacProcessInfo::process_id() const {
    return pid_;
}


int64_t MacProcessInfo::physical_memory() const {
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO, 
                 (task_info_t)&info, &count) == KERN_SUCCESS) {
        return info.phys_footprint;
    }
    return 0;
}

int64_t MacProcessInfo::resident_size() const {
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO, 
                 (task_info_t)&info, &count) == KERN_SUCCESS) {
        return info.resident_size;
    }
    return 0;
}

int64_t MacProcessInfo::virtual_size() const {
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO,
                 (task_info_t)&info, &count) == KERN_SUCCESS) {
        return info.virtual_size;
    }
    return 0;
}

// 新增CPU时间获取
int64_t MacProcessInfo::cpu_user_time() const {
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec; // 微秒
    }
    return 0;
}

int64_t MacProcessInfo::cpu_system_time() const {
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec;
    }
    return 0;
}

// 新增线程数统计
int64_t MacProcessInfo::thread_count() const {
    task_t task;
    mach_msg_type_number_t count;
    thread_act_array_t list;
    if (task_for_pid(mach_task_self(), pid_, &task) == KERN_SUCCESS) {
        if (task_threads(task, &list, &count) == KERN_SUCCESS) {
            vm_deallocate(mach_task_self(), (vm_address_t)list, sizeof(thread_t) * count);
            return count;
        }
    }
    return 0;
}

// 新增进程启动时间
int64_t MacProcessInfo::start_timestamp() const {
    struct kinfo_proc proc_info = {};
    size_t len = sizeof(proc_info);
    int mib[] = { 
        CTL_KERN, 
        KERN_PROC, 
        KERN_PROC_PID, 
        static_cast<int>(pid_)
    };
    
    if (sysctl(mib, sizeof(mib)/sizeof(mib[0]), &proc_info, &len, NULL, 0) != 0) {
        return 0;
    }
    
    if (len != sizeof(proc_info) || proc_info.kp_proc.p_flag & P_SYSTEM) {
        return 0;
    }

    const auto& starttime = proc_info.kp_proc.p_starttime;
    return static_cast<int64_t>(starttime.tv_sec) * 1000LL 
         + starttime.tv_usec / 1000LL;
}

} // namespace detail
} // namespace cxxtrace