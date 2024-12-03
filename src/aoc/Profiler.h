#pragma once
#include <chrono>

namespace aoc {
    class Profiler {
    public:
        Profiler() = delete;

        ~Profiler() = delete;

        template<typename Func>
        static auto profile(Func &&f, const size_t runs = 1000) {
            auto total = std::chrono::microseconds(0);
            for (size_t i = 0; i < runs; ++i) {
                auto start = std::chrono::steady_clock::now();
                f();
                auto end = std::chrono::steady_clock::now();
                total += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            }
            return total / runs;
        }
    };
}
