// Probably not the best way to do...

#include <mednafen/mednafen.h>
#include <mednafen/general.h>
#include "ss.h"
#include "sound.h"
#include "scsp.h" // For debug.inc
#include "smpc.h"
#include "stvio.h"
#include "cdb.h"
#include "vdp1.h"
#include "vdp2.h"
#include "scu.h"
#include "cart.h"
#include "db.h"

#include <stdint.h>
#include <string>
#include "../drivers_imgui/debugui.h"

using namespace Mednafen;
using namespace MDFN_IEN_SS;

namespace MDFN_IEN_SS
{
#include "sh7095.h"
}

// ================
// Cpu regs (need m68k)
// ================

#define _SH2_REG32(n) {.name = (char *)n, .fmt = REG_FMT_32}

static debugui_cpu_reg_t regs_sh2[] = {
    _SH2_REG32("R00"),
    _SH2_REG32("R01"),
    _SH2_REG32("R02"),
    _SH2_REG32("R03"),
    _SH2_REG32("R04"),
    _SH2_REG32("R05"),
    _SH2_REG32("R06"),
    _SH2_REG32("R07"),
    _SH2_REG32("R08"),
    _SH2_REG32("R09"),
    _SH2_REG32("R10"),
    _SH2_REG32("R11"),
    _SH2_REG32("R12"),
    _SH2_REG32("R13"),
    _SH2_REG32("R14"),
    _SH2_REG32("R15"),
    _SH2_REG32("SR"),
    _SH2_REG32("GBR"),
    _SH2_REG32("VBR"),
    _SH2_REG32("MACH"),
    _SH2_REG32("MACL"),
    _SH2_REG32("PR"),
    _SH2_REG32("PC"),
    // need more ...
};


uint32_t gdb_get_reg(int cpu_n, uint32_t i)
{
    if (i < 16)
        return CPU[cpu_n].R[i];

    switch(i) {
        case 16: return CPU[cpu_n].PC;
        case 17: return CPU[cpu_n].PR;
        case 18: return CPU[cpu_n].GBR;
        case 19: return CPU[cpu_n].VBR;
        case 20: return CPU[cpu_n].MACH;
        case 21: return CPU[cpu_n].MACL;
        case 22: return CPU[cpu_n].SR;
        default:return 0;
    }
}


uint8_t gdb_mem_peek8(uint32_t adr) {   
    return ne16_rbo_be<uint8_t>(SH7095_FastMap[adr >> SH7095_EXT_MAP_GRAN_BITS], adr);
}


uint32_t gdb_mem_peek(uint32_t adr) {
    return ne16_rbo_be<uint32>(SH7095_FastMap[adr >> SH7095_EXT_MAP_GRAN_BITS], adr);
}

static void _sh2_tab(int cpu_n, debugui_cpu_tab_t *tab)
{
    tab->regs = regs_sh2;
    tab->regs_count = sizeof(regs_sh2) / sizeof(regs_sh2[0]);

    int i = 0;
    // r0 - r15
    for (i = 0; i < 16; i++)
    {
        regs_sh2[i].value = CPU[cpu_n].R[i];
    }

    //
    regs_sh2[i++].value = CPU[cpu_n].SR;
    regs_sh2[i++].value = CPU[cpu_n].GBR;
    regs_sh2[i++].value = CPU[cpu_n].VBR;
    regs_sh2[i++].value = CPU[cpu_n].MACH;
    regs_sh2[i++].value = CPU[cpu_n].MACL;
    regs_sh2[i++].value = CPU[cpu_n].PR;
    regs_sh2[i++].value = CPU[cpu_n].PC;
}

void debugui_get_cpu(int cpu_n, debugui_cpu_tab_t *tab)
{
    switch (cpu_n)
    {
    case 0:
        _sh2_tab(0, tab);
        break;
    case 1:
        _sh2_tab(1, tab);
        break;

    default:
        break;
    }
}

// ================
// VDP2 Regs
// ================

#define _VDP2_REG16(a, n) {.adr = (char *)a, .name = (char *)n, .fmt = REG_FMT_16}

static debugui_dev_reg_t regs_common[] = {
    _VDP2_REG16("$25F8_0000", "TVMD"),
    _VDP2_REG16("$25F8_0002", "EXTEN"),
    _VDP2_REG16("$25F8_0004", "TVSTAT"),
    _VDP2_REG16("$25F8_0006", "VRSIZE"),
    _VDP2_REG16("$25F8_0008", "HCNT"),
    _VDP2_REG16("$25F8_000A", "VCNT"),
    _VDP2_REG16("$25F8_000C", "RES0"),
    _VDP2_REG16("$25F8_000E", "RAMCTL"),
};

static void debugui_get_dev_common_vdp2_regs(debugui_reg_tab_t *tab)
{
    tab->regs = regs_common;
    tab->regs_count = sizeof(regs_common) / sizeof(regs_common[0]);
    //===
    // tvmd
    //===
    std::string tvmd = "";
    switch (VDP2::RawRegs[0] & 0x7)
    {
    case 0:
    case 4:
        tvmd += "320";
        break;
    case 1:
    case 5:
        tvmd += "352";
        break;
    case 2:
    case 6:
        tvmd += "640";
        break;
    case 3:
    case 7:
        tvmd += "704";
        break;
    }
    switch ((VDP2::RawRegs[0] >> 4) & 0x3)
    {
    case 0:
        tvmd += "x224";
        break;
    case 1:
        tvmd += "x240";
        break;
    case 2:
        tvmd += "x256";
        break;
    }

    switch ((VDP2::RawRegs[0] >> 6) & 0x3)
    {
    case 0:
        tvmd += " Non-Interlace";
        break;
    case 2:
        tvmd += " Single-Density Interlace";
        break;
    case 3:
        tvmd += " Double-Density Interlace";
        break;
    }

    //
    regs_common[0].dec = tvmd;
    regs_common[0].value = VDP2::RawRegs[0];

    //===
    // EXTEN
    //===
    regs_common[1].value = VDP2::RawRegs[1]; /*EXTEN*/

    //===
    // TVSTAT
    //===
    regs_common[2].value = VDP2::RawRegs[2]; /*TVSTAT*/

    //===
    // VRSIZE
    //===
    regs_common[3].value = VDP2::RawRegs[3]; /*VRSIZE*/

    //===
    // HCNT
    //===
    regs_common[4].value = VDP2::PeekHPos(); /*HCNT*/

    //===
    // VCNT
    //===
    regs_common[5].value = VDP2::PeekLine(); /*VCNT*/

    //===
    // RES0
    //===
    regs_common[6].value = VDP2::RawRegs[6]; /*RES0*/

    //===
    // RAMCTL
    //===
    regs_common[7].value = VDP2::RawRegs[7]; /*RAMCTL*/
}

void debugui_get_dev_regs(int n, debugui_reg_tab_t *tab)
{
    switch (n)
    {
        // Common
    case 0:
        debugui_get_dev_common_vdp2_regs(tab);
        break;

    default:
        break;
    }
}