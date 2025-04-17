

#include "main.h"
#include <mednafen/FileStream.h>

#include <trio/trio.h>
#include <map>
#include "debugger.h"
#include "gfxdebugger.h"
#include "memdebugger.h"
#include "logdebugger.h"
#include "prompt.h"
#include "video.h"

#include "gdb.h"

void med_gdb_report_pc(uint32_t pc)
{
    server.reportPC(pc);

    while (server.isHalted())
    {
        DebuggerFudge();
    }
}

void med_gdb_update()
{
    server.updateLoop();
}

extern uint32_t gdb_get_reg(int cpu_n, uint32_t i);
extern uint8_t gdb_mem_peek8(uint32_t adr);
extern uint32_t gdb_mem_peek(uint32_t adr);

void med_init_gdb()
{
    server.reset();

    server.hooks.regRead = [](u32 regIdx)
    {
        return hex(gdb_get_reg(0, regIdx), 8, '0');
    };

    server.hooks.regReadGeneral = []()
    {
        std::string res{};
        // for (auto i : range(24))
        for (int i = 0; i < 24; i++)
        {
            res.append(server.hooks.regRead(i));
        }
        return res;
    };

    server.hooks.regWrite = [](u32 regIdx, u64 regValue) -> bool
    {
        return false; // not impl
    };

    server.hooks.read = [](u64 address, u32 byteCount) -> std::string
    {
        address = (s32)address;

        std::string res{};
        res.resize(byteCount * 2);
        char *resPtr = (char*)res.c_str();

        for (u32 i = 0; i < byteCount; i++)
        {
            u8 val = gdb_mem_peek8(address++);
            hexByte(resPtr, val);
            resPtr += 2;
        }
        return res;
    };

    server.hooks.write = [](u64 address, std::vector<u8> data)
    {
        address = (s32)address;
        // not impl
    };

    server.hooks.normalizeAddress = [](u64 address)
    {
        return address & 0x0FFF'FFFF;
    };

    server.hooks.targetXML = []() -> std::string
    {
        return "<target version=\"1.0\">"
               "<architecture>sh2</architecture>"
               "</target>";
    };

    server.open(43434, true);
}
