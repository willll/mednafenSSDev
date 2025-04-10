#pragma once

#include <functional>

class DBGProfiler
{
public:
    uint32_t cycles[2];
    void init();
    void start(int cpu_n, uint32_t pc);
    void end(int cpu_n, uint32_t pc);
    void reset();
    void frame();
    void cycle(int n, uint32_t c) { cycles[n] = c; }
    std::function<void(uint32_t adr, uint64_t cycles_count, uint64_t call_count)> cb{};
};

extern DBGProfiler dbg_profiler;