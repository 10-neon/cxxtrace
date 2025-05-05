
#include <linux/perf_event.h>
#include <unistd.h>

#include <cstdint>
#include <memory>
#include <string>

namespace neon {

class PerfEvent {
   public:
    enum Domain : std::uint8_t {
        USER = 0b1,
        KERNEL = 0b10,
        HYPERVISOR = 0b100,
        IDLE = 0b1000,
        ALL = 0b1111
    };
    enum class TypeID : std::uint32_t {
        HARDWARE = PERF_TYPE_HARDWARE,
        SOFTWARE = PERF_TYPE_SOFTWARE,
        TRACEPOINT = PERF_TYPE_TRACEPOINT,
        HW_CACHE = PERF_TYPE_HW_CACHE,
        RAW = PERF_TYPE_RAW,
        BREAKPOINT = PERF_TYPE_BREAKPOINT,
    };
    enum class Config : std::uint64_t {
        HW_CPU_CYCLES = PERF_COUNT_HW_CPU_CYCLES,
        HW_INSTRUCTIONS = PERF_COUNT_HW_INSTRUCTIONS,
        SW_CPU_CLOCK = PERF_COUNT_SW_CPU_CLOCK,
        SW_TASK_CLOCK = PERF_COUNT_SW_TASK_CLOCK,
        SW_DUMMY = PERF_COUNT_SW_DUMMY,
    };
    struct Count {
        std::uint64_t value;
        // std::uint64_t time_enabled;
        // std::uint64_t time_running;

        double operator-(const Count& other) const;
    };

    struct Option {
        int fd{-1};
        int id{0};
        TypeID type;
        Config config;
        Domain domain;
    };
    PerfEvent(const Option& option);
    PerfEvent(const PerfEvent&) = delete;
    PerfEvent(PerfEvent&& other);
    ~PerfEvent();
    PerfEvent& operator=(const PerfEvent& other) = delete;
    PerfEvent& operator=(PerfEvent&& other);

    std::string name() const;
    static std::unique_ptr<PerfEvent> create(TypeID type, Config config,
                                             Domain domain);
    void enable() const;
    bool now(Count& count) const;
    void disable() const;
    int id() const noexcept;
    TypeID type() const noexcept;
    Config config() const noexcept;
    Domain domain() const noexcept;

   private:
    static const std::string& to_string(TypeID type_id);
    static const std::string& to_string(TypeID type_id, Config config);
    static std::string domainName(Domain domain);
    Option self_;
};
}  // namespace neon
