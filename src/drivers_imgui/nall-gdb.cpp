

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

#include "nall-gdb.h"

// strange...
#include <nall/stdint.hpp>
#include <nall/string.hpp>
#include <nall/chrono.hpp>
#include <nall/vector.hpp>

using namespace nall;
#include <nall/gdb/server.cpp>
#include <nall/tcptext/tcp-socket.cpp>
#include <nall/tcptext/tcptext-server.cpp>

void med_gdb_report_pc(uint32_t pc)
{
    GDB::server.reportPC(pc);

    while (GDB::server.isHalted())
    {
        DebuggerFudge();
    }
}

void med_gdb_update()
{
    nall::GDB::server.updateLoop();
}

extern uint32_t gdb_get_reg(int cpu_n, uint32_t i);
extern uint8_t gdb_mem_peek8(uint32_t adr);
extern uint32_t gdb_mem_peek(uint32_t adr);

void med_init_gdb()
{
    nall::GDB::server.reset();

    GDB::server.hooks.regRead = [](u32 regIdx)
    {
        return hex(gdb_get_reg(0, regIdx), 8, '0');
    };

    GDB::server.hooks.regReadGeneral = []()
    {
        string res{};
        for (auto i : range(24))
        {
            res.append(GDB::server.hooks.regRead(i));
        }
        return res;
    };

    GDB::server.hooks.regWrite = [](u32 regIdx, u64 regValue) -> bool
    {
        return false; // not impl
    };

    GDB::server.hooks.read = [](u64 address, u32 byteCount) -> string
    {
        address = (s32)address;

        string res{};
        res.resize(byteCount * 2);
        char *resPtr = res.begin();

        for (u32 i : range(byteCount))
        {
            u8 val = gdb_mem_peek8(address++);
            hexByte(resPtr, val);
            resPtr += 2;
        }
        return res;
    };

    GDB::server.hooks.write = [](u64 address, vector<u8> data)
    {
        address = (s32)address;
        // not impl
    };

    GDB::server.hooks.normalizeAddress = [](u64 address)
    {
        return address & 0x0FFF'FFFF;
    };

    GDB::server.hooks.targetXML = []() -> string
    {
        return "<target version=\"1.0\">"
               "<architecture>sh2</architecture>"
               "</target>";
    };

    nall::GDB::server.open(43434, true);
}
