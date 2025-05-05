#include "perf_event.h"

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include <cassert>
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace neon {
long perf_event_open(struct perf_event_attr* hw_event, pid_t pid, int cpu,
                     int group_fd, unsigned long flags) {
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

PerfEvent::PerfEvent(const Option& option) : self_{option} {}

PerfEvent::PerfEvent(PerfEvent&& other) {
    if (self_.fd > 0) {
        close(self_.fd);
    }
    self_ = other.self_;
    other.self_.fd = -1;
}

PerfEvent::~PerfEvent() {
    if (self_.fd > 0) {
        ioctl(self_.fd, PERF_EVENT_IOC_DISABLE, 0);
        close(self_.fd);
    }
}

PerfEvent& PerfEvent::operator=(PerfEvent&& other) {
    if (this == &other) {
        return *this;
    }
    if (self_.fd != other.self_.fd && self_.fd > 0) {
        close(self_.fd);
    }
    self_ = other.self_;
    other.self_.fd = -1;
    return *this;
}
const std::string& PerfEvent::to_string(TypeID type_id) {
    static const std::unordered_map<TypeID, std::string> names{
        {TypeID::HARDWARE, "HARDWARE"},
        {TypeID::SOFTWARE, "SOFTWARE"},
        {TypeID::TRACEPOINT, "TRACEPOINT"},
        {TypeID::HW_CACHE, "HW_CACHE"},
        {TypeID::RAW, "RAW"},
        {TypeID::BREAKPOINT, "BREAKPOINT"},
    };
    return names.at(type_id);
}

const std::string& PerfEvent::to_string(TypeID type_id, Config config) {
    static const std::unordered_map<TypeID,
                                    std::unordered_map<Config, std::string>>
        names{
            {TypeID::HARDWARE,
             {
                 {Config::HW_CPU_CYCLES, "HW_CPU_CYCLES"},
                 {Config::HW_INSTRUCTIONS, "HW_INSTRUCTIONS"},
             }},
            {TypeID::SOFTWARE,
             {
                 {Config::SW_CPU_CLOCK, "SW_CPU_CLOCK"},
                 {Config::SW_TASK_CLOCK, "SW_TASK_CLOCK"},
                 {Config::SW_DUMMY, "SW_DUMMY"},
             }},
        };
    return names.at(type_id).at(config);
}

std::string PerfEvent::domainName(Domain domain) {
    std::string domain_name = "(";
    if (domain & USER) {
        domain_name += "usr:";
    }
    if (domain & KERNEL) {
        domain_name += "sys:";
    }
    if (domain & IDLE) {
        domain_name += "idle:";
    }
    if (domain & HYPERVISOR) {
        domain_name += "hv:";
    }
    return domain_name + ")";
}

std::string PerfEvent::name() const {
    return to_string(self_.type) + ":" + to_string(self_.type, self_.config) +
           ":" + domainName(self_.domain) + ":" + std::to_string(self_.id);
}

std::unique_ptr<PerfEvent> PerfEvent::create(TypeID type, Config config,
                                             Domain domain) {
    Option option;
    option.type = type;
    option.config = config;
    option.domain = domain;
    struct perf_event_attr attr;
    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = static_cast<std::uint32_t>(type);
    attr.size = sizeof(struct perf_event_attr);
    attr.config = static_cast<std::uint64_t>(config);
    attr.disabled = true;
    attr.exclude_user = !(domain & USER);
    attr.exclude_kernel = !(domain & KERNEL);
    attr.exclude_idle = !(domain & IDLE);
    attr.exclude_hv = !(domain & HYPERVISOR);
    // attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
    // PERF_FORMAT_TOTAL_TIME_RUNNING;
    attr.read_format = 0;

    option.fd = static_cast<int>(perf_event_open(&attr, 0, -1, -1, 0));
    if (option.fd < 0) {
        return nullptr;
    }
    ioctl(option.fd, PERF_EVENT_IOC_ID, &option.id);

    return std::make_unique<PerfEvent>(option);
}

void PerfEvent::enable() const {
    assert(ioctl(self_.fd, PERF_EVENT_IOC_RESET, 0) == 0);
    assert(ioctl(self_.fd, PERF_EVENT_IOC_ENABLE, 0) == 0);
}

bool PerfEvent::now(Count& count) const {
    int bytes = read(self_.fd, &count, sizeof(Count));
    return bytes == sizeof(Count);
}

void PerfEvent::disable() const {
    assert(ioctl(self_.fd, PERF_EVENT_IOC_DISABLE, 0) == 0);
}

int PerfEvent::id() const noexcept { return self_.id; }

PerfEvent::TypeID PerfEvent::type() const noexcept { return self_.type; }

PerfEvent::Config PerfEvent::config() const noexcept { return self_.config; }

PerfEvent::Domain PerfEvent::domain() const noexcept { return self_.domain; }
}  // namespace neon
