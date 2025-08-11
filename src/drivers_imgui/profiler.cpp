#include <stdint.h>
#include <stdio.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "profiler.h"
#include <mutex>

DBGProfiler dbg_profiler;

struct profile_item_t
{
    uint32_t address;
    uint64_t total_cycles_count;
    uint64_t total_call_count;
    uint64_t frame_call_count;
    uint64_t frame_cycles_count;

    uint64_t cycles_start;
    uint64_t cycles_end;

    profile_item_t()
        : address(0), total_cycles_count(0), total_call_count(0), frame_call_count(0), frame_cycles_count(0), cycles_start(0), cycles_end(0)
    {
    }

    void resetFrame()
    {
        this->frame_call_count = 0;
        // do not reset in mid frame
        if (cycles_end > cycles_start)
        {
            this->frame_cycles_count = 0;
        }
    }

    void reset()
    {
        this->total_cycles_count = 0;
        this->total_call_count = 0;
        this->frame_call_count = 0;
        this->frame_cycles_count = 0;
        this->cycles_start = 0;
        this->cycles_end = 0;
    }
};

struct stack_item_t
{
    explicit stack_item_t(uint32_t a, uint64_t c) : adr(a), cycle_start(c) {}
    uint32_t adr;
    uint64_t cycle_start;

    void resetCycle()
    {
        this->cycle_start = 0;
    }
};

static std::stack<stack_item_t> call_stack;
static std::unordered_map<uint32_t, profile_item_t> profile_stack;
static std::mutex profiler_mutex; // Add this line

void DBGProfiler::init()
{
    reset();
    // call_stack
}

void DBGProfiler::reset()
{
    std::lock_guard<std::mutex> lock(profiler_mutex);
    for (auto &data : profile_stack)
    {
        data.second.reset();
        // data.second.resetFrame();
    }
    std::stack<stack_item_t> temp_stack;
    while (!call_stack.empty())
    {
        stack_item_t item = call_stack.top();
        call_stack.pop();
        item.resetCycle();
        temp_stack.push(item);
    }

    // Swap the stacks
    while (!temp_stack.empty())
    {
        call_stack.push(temp_stack.top());
        temp_stack.pop();
    }
}

void DBGProfiler::start(int cpu_n, uint32_t address)
{
    std::lock_guard<std::mutex> lock(profiler_mutex);
    call_stack.push(stack_item_t(address, cycles[cpu_n]));
    profile_stack[address].address = address;
    profile_stack[address].cycles_start = cycles[cpu_n];
    profile_stack[address].total_call_count++;
    profile_stack[address].frame_call_count++;
}

__attribute__((optimize("O0"))) void DBGProfiler::end(int cpu_n, uint32_t pc)
{
    std::lock_guard<std::mutex> lock(profiler_mutex);
    stack_item_t stack = call_stack.top();
    uint64_t d = cycles[cpu_n] - stack.cycle_start;

    profile_stack[stack.adr].total_cycles_count += d;
    profile_stack[stack.adr].frame_cycles_count += d;
    profile_stack[stack.adr].cycles_end = cycles[cpu_n];

    call_stack.pop();
}

void DBGProfiler::frame()
{
    std::lock_guard<std::mutex> lock(profiler_mutex);
    if (cb)
    {
        std::vector<std::pair<uint32_t, profile_item_t>> vec;
        std::copy_if(profile_stack.begin(), profile_stack.end(), std::back_inserter(vec),
                     [](const std::pair<uint32_t, profile_item_t> &item)
                     {
                         return item.second.frame_call_count > 0;
                     });

        std::sort(vec.begin(), vec.end(), [](const std::pair<uint32_t, profile_item_t> &a, const std::pair<uint32_t, profile_item_t> &b)
                  { return a.second.frame_call_count > b.second.frame_call_count; });

        for (const auto &data : vec)
        {
            cb(data.second.address, data.second.frame_cycles_count, data.second.frame_call_count);
        }
    }
}