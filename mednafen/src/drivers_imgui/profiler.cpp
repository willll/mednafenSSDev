#include <stdint.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "profiler.h"

DBGProfiler dbg_profiler;

typedef struct
{
    uint32_t address;
    uint64_t total_cycles_count;
    uint64_t total_call_count;
    uint64_t frame_call_count;
    uint64_t frame_cycles_count;

    void resetFrame()
    {
        this->frame_call_count = 0;
        this->frame_cycles_count = 0;
    }
} profile_item_t;

static std::stack<uint32_t> call_stack;
static std::unordered_map<uint32_t, profile_item_t> profile_stack;

void DBGProfiler::init()
{
    // call_stack
}

void DBGProfiler::reset()
{
    //  profile_stack.clear();
}

void DBGProfiler::start(int cpu_n, uint32_t address)
{
    call_stack.push(address);
    profile_stack[address].address = address;
    profile_stack[address].address = address;
    profile_stack[address].total_call_count++;
    profile_stack[address].frame_call_count++;
}

void DBGProfiler::end(int cpu_n, uint32_t pc)
{
    uint32_t address = call_stack.top();
    call_stack.pop();
    // add time ?
}

void DBGProfiler::frame()
{
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
    for (auto &data : profile_stack)
    {
        data.second.resetFrame();
    }
}