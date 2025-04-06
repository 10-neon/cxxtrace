#include "cxxtrace/cxxtrace.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <pthread.h>

// 模拟数据库查询
std::vector<float> query_database(int user_id) {
    TRACE_SCOPE(DatabaseQuery);
    std::vector<float> transaction_records;
    
    // 模拟查询延迟
    { 
        cxxtrace::Scope delay_scope("DB_Latency", ::nostd::source_location::current());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 生成随机交易数据
    std::mt19937 gen(user_id);
    std::uniform_real_distribution<float> amount_dist(100.0, 5000.0);
    for (int i = 0; i < 1000; ++i) {
        transaction_records.emplace_back(amount_dist(gen));
    }
    
    return transaction_records;
}

// 财务分析算法
float analyze_transactions(const std::vector<float>& records) {
    TRACE_SCOPE(FinancialAnalysis);
    float total = 0.0f;
    
    // 内存敏感操作
    auto buffer = std::make_unique<float[]>(records.size());
    {
        TRACE_SCOPE(DataProcessing);
        for (size_t i = 0; i < records.size(); ++i) {
            buffer[i] = records[i] * 1.1f; // 增值税计算
            total += buffer[i];
        }
    }
    
    // CPU密集型计算
    {
        TRACE_SCOPE(StatisticalCalculation);
        volatile int prime_count = 0; // 防止优化
        
        // 新增质数计算负载（计算1,000,000以内的质数）
        for(int n = 2; n < 1000000; ++n) {
            bool is_prime = true;
            for(int i = 2; i*i <= n; ++i) {
                if(n % i == 0) {
                    is_prime = false;
                    break;
                }
            }
            if(is_prime) {
                ++prime_count;
            }
        }
        
        // 保留原有计算逻辑
        volatile float checksum = 0;
        for (int i = 0; i < 1000000; ++i) {
            checksum += std::sqrt(total) * i;
        }
    }
    
    return total;
}

void process_user(int user_id) {
    TRACE_SCOPE(UserProcessing);
    auto records = query_database(user_id);
    auto total = analyze_transactions(records);
    
    std::cout << "用户" << user_id << " 总交易额（含税）: ¥" << total << std::endl;
}

int main() {
    TRACE_SCOPE(MainProgram);
    pthread_setname_np("Main Thread");
    
    // 启动工作线程
    std::thread finance_thread([]{
        TRACE_SCOPE(FinanceDepartment);
        process_user(1001);
        process_user(1002);

    });
    
    // 主线程处理
    process_user(1003);
    
    finance_thread.join();
    
    // 生成性能报告
    std::ofstream perf_report("scope.json");
    perf_report << cxxtrace::TraceDumper::dumpJson().dump(4);
    perf_report.close();
    std::cout << "性能分析报告已生成: scope.json" << std::endl;
    
    return 0;
}