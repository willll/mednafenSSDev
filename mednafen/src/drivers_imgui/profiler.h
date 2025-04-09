#pragma once

#include <functional>


class DBGProfiler {
    public:
    void init();
    void start(int cpu_n, uint32_t pc);
    void end(int cpu_n, uint32_t pc);
    void reset();
    void frame();
    std::function<void(uint32_t adr, uint32_t cycles_count, uint32_t call_count)> cb{};
};

extern DBGProfiler dbg_profiler;