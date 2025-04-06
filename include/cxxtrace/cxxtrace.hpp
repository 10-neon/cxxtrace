/** @file
 * @brief C++性能追踪库核心头文件
 * @defgroup CxxTraceAPI 核心追踪接口
 */

#pragma once
#include "utils/wrap.h"
#include "utils/source_location.hpp"
#include "utils/json.hpp"
#include <array>
#include <cstdint>
#include <map>
#include <mutex>
#include <stack>
#include <list>
#include <memory>
#include "cxxtrace/detail/thread_info.h"
#include "cxxtrace/detail/process_info.h"
#include <sstream>

namespace cxxtrace {

using namespace nostd;
using namespace nlohmann;

// compile time string
using Tag = const char*;

/** 
 * @class Counter
 * @brief 性能计数器核心类
 * @ingroup CxxTraceAPI
 * 
 * 负责收集和计算以下性能指标：
 * - CPU用户态时间
 * - CPU内核态时间
 * - 挂钟时间
 * - 堆内存分配/释放量
 */
class Counter {
public:
    /** 
     * @enum Field
     * @brief 性能计数器指标类型枚举
     */
    enum class Field {
        kCPUUserTime,    ///< 用户态CPU时间（豪秒）
        kCPUSystemTime,  ///< 内核态CPU时间（豪秒）
        kWallClockTime,  ///< 挂钟时间（豪秒）
        kHeapAlloc,      ///< 堆内存分配总量（字节）
        kHeapDealloc,    ///< 堆内存释放总量（字节）
        kEnd             ///< 枚举结束标记（内部使用）
    };
    using value_type = std::uint64_t;
    Counter() {
        for (auto& value : values_) {
            value = 0;
        }
    }
    Counter operator+(const Counter& other) const {
        Counter result(*this);
        result += other;
        return result;
    }
    Counter operator-(const Counter& other) const {
        Counter result(*this);
        result -= other;
        return result;
    }
    Counter& operator+=(const Counter& other) {
        for (int i = 0; i < static_cast<int>(Field::kEnd); ++i) {
            values_[i] += other.values_[i];
        }
        return *this;
    }
    Counter& operator-=(const Counter& other) {
        for (int i = 0; i < static_cast<int>(Field::kEnd); ++i) {
            values_[i] -= other.values_[i];
        }
        return *this;
    }

    /**
     * @brief 获取当前线程的瞬时计数器快照
     * @param thread_info 当前线程信息对象
     * @return 包含当前时刻性能指标的快照对象
     */
    static std::unique_ptr<Counter> now(const std::shared_ptr<detail::ThreadInfo>& thread_info) {
        auto counter = std::make_unique<Counter>();
        if (thread_info) {
            thread_info->update();
            counter->values_[static_cast<int>(Field::kCPUUserTime)] = thread_info->cpu_user_time();
            counter->values_[static_cast<int>(Field::kCPUSystemTime)] = thread_info->cpu_system_time();
        }
        return counter;
    }
    value_type& value(Field field) {
        return values_[static_cast<int>(field)];
    }
    value_type const& value(Field field) const {
        return values_[static_cast<int>(field)];
    }
private:
    std::array<value_type, static_cast<int>(Field::kEnd)> values_{};
};

/**
 * @struct ScopeNode
 * @brief 表示作用域节点的数据结构
 * @ingroup CxxTraceAPI
 * 
 * 用于构建作用域调用图，包含：
 * - 源代码位置信息
 * - 开发者定义的作用域标签
 */
struct ScopeNode {
    source_location loc; ///< 源代码位置（自动捕获）
    Tag tag = nullptr;   ///< 作用域标签（用户定义）
    
    /// 比较运算符（用于map排序）
    bool operator<(const ScopeNode& other) const {
        if (tag != other.tag) {
            return tag < other.tag;
        }
        if (loc.file_name() != other.loc.file_name()) {
            return loc.file_name() < other.loc.file_name();
        }
        if (loc.line() != other.loc.line()) {
            return loc.line() < other.loc.line();
        }
        return loc.column() < other.loc.column();
    }
};

/**
 * @struct ScopeCounter
 * @brief 作用域级别的性能计数器
 * @extends Counter
 * @ingroup CxxTraceAPI
 * 
 * 扩展基础计数器功能，增加状态管理：
 * - 跟踪正在进行的计数器状态
 * - 支持时间段的累计计算
 */
struct ScopeCounter : public Counter{
    ScopeCounter() = default;
    bool isCompleted() const {
        return ongoing == nullptr;
    }
    void start(const std::shared_ptr<detail::ThreadInfo>& thread_info) {
        ongoing = Counter::now(thread_info);
    }

    void stop(const std::shared_ptr<detail::ThreadInfo>& thread_info) {
        if (ongoing) {
            *this += *Counter::now(thread_info) - *ongoing;
            ongoing.reset();
        }
    }
    void update(const std::shared_ptr<detail::ThreadInfo>& thread_info) {
        if (ongoing) {
            auto now_counter = Counter::now(thread_info);
            *this += *now_counter - *ongoing;
            ongoing = std::move(now_counter);
        }
    }
private:
    std::shared_ptr<Counter> ongoing = nullptr;
};

/// 作用域调用图类型定义（父节点 -> 子节点集合 -> 性能计数器）
using ScopeGraph = std::map<ScopeNode, std::map<ScopeNode, ScopeCounter>>;

/**
 * @struct ScopeContext
 * @brief 作用域上下文记录结构
 * @ingroup CxxTraceAPI
 * 
 * 用于线程调用栈中维护当前作用域状态：
 * - 当前作用域节点
 * - 关联的性能计数器引用
 */
struct ScopeContext {
    ScopeNode node;        ///< 当前作用域节点
    ScopeCounter& counter; ///< 关联的性能计数器
};

/**
 * @class ThreadContext
 * @brief 线程级性能数据上下文
 * @ingroup CxxTraceAPI
 * 
 * 管理单个线程的性能数据收集，包含：
 * - 线程专属的性能计数器
 * - 作用域调用图
 * - 线程安全的访问控制
 */
class ThreadContext {
public:
    /// 初始化线程上下文并建立根作用域
    ThreadContext(): thread_info_{detail::ThreadInfo::current_thread()} {
        scope_stack_.push(ScopeContext{.node = ScopeNode{.loc=source_location::current(), .tag = "root"}, .counter=root_counter_});
        // 修复问题：按照声明顺序初始化 ScopeNode 的字段
        enter(ScopeNode{.loc = source_location::current(), .tag = "default"});
    }
    ~ThreadContext() {
        exit();
        thread_info_->finalize();
    }
    
    void enter(const ScopeNode &node) {
        std::lock_guard<std::mutex> lock(mutex_);
        ScopeCounter& scope_counter = scope_graph_[scope_stack_.top().node][node];
        scope_counter.start(thread_info_);
        scope_stack_.push({node, scope_counter});
    }

    void exit() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& current = scope_stack_.top();
        current.counter.stop(thread_info_);
        scope_stack_.pop();
    }

    ScopeGraph graph() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return scope_graph_;
    }
    std::shared_ptr<detail::ThreadInfo> thread_info() const noexcept {
        return thread_info_;
    }

    const std::shared_ptr<detail::ThreadInfo> thread_info_;
    mutable std::mutex mutex_;
    ScopeGraph scope_graph_;
    ScopeCounter root_counter_;
    std::stack<ScopeContext> scope_stack_;
};

/** 
 * @brief 进程上下文信息收集类
 * @ingroup CxxTraceAPI
 * 
 * 该类负责收集和维护以下信息：
 * - 进程级CPU/内存使用情况
 * - 所有线程的追踪上下文
 * - 线程安全的上下文访问控制
 */
class ProcessContext  {
public:
    static ProcessContext& inst() {
        static ProcessContext context;
        return context;
    }
    
    ThreadContext& current() {
        thread_local static ThreadContext* thread_context = [this]() {
            std::lock_guard<std::mutex> lock(all_thread_context_mutex_);
            all_thread_context_.emplace_back();
            return &all_thread_context_.back();
        }();
        return *thread_context;
    }

    std::list<std::pair<std::shared_ptr<detail::ThreadInfo>, ScopeGraph>> threads() const {
        std::list<std::pair<std::shared_ptr<detail::ThreadInfo>, ScopeGraph>> result;
        for (auto const& ctx : all_thread_context_) {
            result.emplace_back(ctx.thread_info(), ctx.graph());
        }
        return result;
    }

    detail::ProcessInfo& process_info() const {
        return *process_info_;
    }

private:
    ProcessContext(): process_info_{detail::ProcessInfo::create()} {}
    std::unique_ptr<detail::ProcessInfo> process_info_;
    std::mutex all_thread_context_mutex_;
    std::list<ThreadContext> all_thread_context_;
};

/**
 * @brief 进入新的性能追踪作用域
 * @ingroup CxxTraceAPI
 * @param tag 作用域标识标签
 * @param loc 源代码位置（自动捕获）
 * 
 * 示例用法：
 * @code{.cpp}
 * beginScopeSection("DatabaseQuery", source_location::current());
 * @endcode
 */
void beginScopeSection(Tag tag, source_location loc);

/**
 * @brief 退出当前性能追踪作用域
 * @ingroup CxxTraceAPI
 */
void endScopeSection();

/**
 * @brief 自动化作用域追踪RAII类
 * @ingroup CxxTraceAPI
 * 
 * 构造时自动调用beginScopeSection，析构时自动调用endScopeSection。
 * 推荐使用TRACE_SCOPE宏代替直接实例化。
 */
class Scope {
public:
    /** 
     * @brief 构造函数启动作用域追踪
     * @param tag 作用域标识名称
     * @param loc 自动捕获的源代码位置
     */
    Scope(const Tag tag, const source_location &loc) {
        beginScopeSection(tag, loc);
    }

    /// 析构函数结束作用域追踪
    ~Scope() {
        endScopeSection();
    }
};

template <typename Pointer>
using TracePtr = ParametricWrapPtr<Pointer, void(*)(), void(*)()>;

template <typename Pointer>
TracePtr<Pointer> traceWrap(Tag tag, source_location loc, Pointer pointer = nullptr) {
  return {pointer, [tag, loc](){ beginScopeSection(tag, loc); }, [](){ endScopeSection(); }};
}

#define TRACE_SCOPE(tag) ::cxxtrace::Scope tag##_scope(#tag, ::nostd::source_location::current());


class TraceDumper {
public:
    static json dumpJson() {
        json result;
        auto& process = ProcessContext::inst();
        
        // 元数据
        result["metadata"] = {
            {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()},
            {"schema_version", "1.0-cxxtrace"}
        };

        // 进程信息
        auto& process_info = process.process_info();
        result["process"] = {
            {"pid", process_info.process_id()},
            {"memory_usage", process_info.physical_memory()},
            {"cpu_usage", {
                {"user", process_info.cpu_user_time()},
                {"system", process_info.cpu_system_time()}
            }}
        };

        // 线程列表
        result["threads"] = json::array();
        
        // 遍历所有线程上下文
        for (auto& thread_info_with_graph : process.threads()) {
            auto& thread_info = thread_info_with_graph.first;
            auto& graph = thread_info_with_graph.second;
            json thread_json;
            // 线程基本信息
            thread_json["id"] = thread_info->thread_id();
            thread_json["name"] = thread_info->thread_name();
            thread_json["cpu_usage"] = {
                {"user", thread_info->cpu_user_time()},
                {"system", thread_info->cpu_system_time()}
            };
            
            // 序列化线程专属的作用域图
            serializeThreadGraph(thread_info, graph, thread_json);
            
            result["threads"].push_back(thread_json);
        }

        return result;
    }

private:

    static void serializeThreadGraph(std::shared_ptr<detail::ThreadInfo> thread_info, ScopeGraph& graph, json& output) {
        json nodes = json::array();
        json edges = json::array();
        std::unordered_map<std::string, std::size_t> node_id_map;

        for ( auto& parent_entry : graph) {
             auto& parent = parent_entry.first;
             auto& children = parent_entry.second;
            auto parent_id = registerNode(parent, nodes, node_id_map);
            
            for ( auto& child_entry : children) {
                 auto& child = child_entry.first;
                 auto& counter = child_entry.second;
                auto child_id = registerNode(child, nodes, node_id_map);
                counter.update(thread_info);
                edges.push_back({
                    {"from", parent_id},
                    {"to", child_id},
                    {"metrics", serializeCounter(counter)}
                });
            }
        }
        
        output["graph"] = {
            {"nodes", nodes},
            {"edges", edges}
        };
    }

    static std::size_t registerNode(const ScopeNode& node, json& nodes,
                          std::unordered_map<std::string, std::size_t>& id_map) {
        std::ostringstream oss;
        oss << (node.tag ? node.tag : "null") << ":"
            << node.loc.file_name() << ":"
            << node.loc.line() << ":"
            << node.loc.column();
        std::string key = oss.str();
        
        if (id_map.find(key) == id_map.end()) {
            std::size_t new_id = nodes.size();
            nodes.push_back({
                {"id", new_id},
                {"tag", node.tag},
                {"location", {
                    {"file", node.loc.file_name()},
                    {"line", node.loc.line()},
                    {"column", node.loc.column()}
                }}
            });
            id_map[key] = new_id;
        }
        return id_map[key];
    }

    static json serializeCounter(const ScopeCounter& counter) {
        return {
            {"cpu_user", counter.value(Counter::Field::kCPUUserTime)},
            {"cpu_sys", counter.value(Counter::Field::kCPUSystemTime)},
            {"wall_clock", counter.value(Counter::Field::kWallClockTime)},
            {"heap_alloc", counter.value(Counter::Field::kHeapAlloc)},
            {"heap_dealloc", counter.value(Counter::Field::kHeapDealloc)}
        };
    }
};
} // namespace cxxtrace
