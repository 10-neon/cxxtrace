#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "cxxtrace/cxxtrace.h"
#include "version.h"

using namespace neon;

// 矩阵乘法函数
void matrixMultiplication() {
    TRACE_SCOPE(matrixMultiplication);
    const int size = 200;  // 矩阵大小
    auto A = std::make_unique<std::vector<std::vector<double>>>(
        size, std::vector<double>(size, 1.0));
    auto B = std::make_unique<std::vector<std::vector<double>>>(
        size, std::vector<double>(size, 2.0));
    auto C = std::make_unique<std::vector<std::vector<double>>>(
        size, std::vector<double>(size, 0.0));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                (*C)[i][j] += (*A)[i][k] * (*B)[k][j];
            }
        }
    }
}

// 蒙特卡罗方法计算圆周率
double calculatePi() {
    TRACE_SCOPE(calculatePi);
    const int numPoints = 1e7;  // 采样点数
    int pointsInsideCircle = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < numPoints; ++i) {
        double x = dis(gen);
        double y = dis(gen);
        if (x * x + y * y <= 1.0) {
            pointsInsideCircle++;
        }
    }
    matrixMultiplication();
    free(malloc(200));
    return 4.0 * pointsInsideCircle / numPoints;
}

// 模拟 sys time 开销
void simulateSysTime() {
    TRACE_SCOPE(simulateSysTime);
    const int numIterations = 1000;  // 迭代次数
    const std::string filename = "temp_file.txt";

    for (int i = 0; i < numIterations; ++i) {
        // 打开文件进行写入
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            // 写入一些数据
            outFile
                << "This is a test line for simulating sys time. Iteration: "
                << i << std::endl;
            outFile.close();
        }

        // 打开文件进行读取
        std::ifstream inFile(filename);
        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                // 简单处理读取的数据
                (void)line;  // 避免未使用变量警告
            }
            inFile.close();
        }
    }
}

class TestClass {
   public:
    void doSomething(int n) {
        for (int i = 0; i < n; ++i) {
            matrixMultiplication();
            calculatePi();
        }
    }
    void doAnotherThing() { calculatePi(); }
};

void threadFunction(int thread_id) {
    TRACE_SCOPE(threadFunction);
    std::cerr << "thread " << thread_id << " start\n";

    // 模拟一些用户态计算，进行矩阵乘法
    matrixMultiplication();
    double pi = calculatePi();
    std::cerr << "PI = " << pi << '\n';

    // 模拟内核态开销 sys time
    simulateSysTime();

    {
        TRACE_SCOPE(thread_sleep);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // malloc(100);
        free(malloc(
            100));  // ios、macos目前有比较完备的内存拦截支持，linux目前仅拦截当前可执行文件的内存分配
        std::cerr << "thread " << thread_id << " end\n";
    }
    simulateSysTime();
}

// 集成方式，静态链接cxxtrace即可。无需LD_PRELOAD等额外设置.
int main() {
    std::cout << buildinfo::version << std::endl;  // trace工具版本号

    TraceEnable();      // 开启性能开销追踪
    TRACE_SCOPE(main);  // 统计main scope开销

    // 支持unique_ptr、shared_ptr、裸指针的对象级别开销调用。
    auto tracked_test_obj =
        neon::traceWrap("obj_autotrack", neon::SourceLocation::current(),
                        std::make_unique<TestClass>());
    tracked_test_obj->doSomething(2);  // 这个对象上的调用的开销将自动被追踪。

    std::vector<std::thread> threads;

    // 创建多个线程
    for (int i = 0; i < 6; ++i) {
        std::cerr << "main thread start thread" << i << '\n';
        threads.emplace_back(threadFunction, i);
        tracked_test_obj
            ->doAnotherThing();  // 这个对象上的调用的开销将自动被追踪。
    }
    // 可以和原始unique_ptr、shared_ptr、裸指针兼容，但是转换后将失去开销追踪能力
    std::unique_ptr<TestClass> untrack_test_obj = std::move(tracked_test_obj);

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    calculatePi();
    return 0;
}
