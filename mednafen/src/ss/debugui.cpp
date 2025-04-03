// Probably not the best way to do...


#include <mednafen/mednafen.h>
#include <mednafen/general.h>
#include "ss.h"
#include "sound.h"
#include "scsp.h"	// For debug.inc
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


#define _SH2_REG32(n) {.name = (char*)n, .size = 4}

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

static void _sh2_tab(int cpu_n, debugui_cpu_tab_t *tab)
{
    tab->regs = regs_sh2;
    tab->regs_count = sizeof(regs_sh2)/sizeof(regs_sh2[0]);

    int i = 0;
    // r0 - r15
    for (i =0; i<16;i++) {
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
