#pragma once

//
enum REG_FMT
{
    REG_FMT_32,
    REG_FMT_16,
    REG_FMT_8,
    REG_FMT_BOOL,
};

typedef struct
{
    char *name;
    uint32_t value;
    uint32_t fmt;
} debugui_cpu_reg_t;

typedef struct
{
    debugui_cpu_reg_t *regs;
    size_t regs_count;
} debugui_cpu_tab_t;

//
typedef struct
{
    char *adr;
    char *name;
    uint32_t fmt;
    std::string dec;
    uint32_t value;
} debugui_dev_reg_t;

typedef struct
{
    char *name;
    debugui_dev_reg_t *regs;
    size_t regs_count;
} debugui_reg_tab_t;
//

//
void debugui_get_dev_regs(int n, debugui_reg_tab_t *tab);
void debugui_get_cpu(int cpu_n, debugui_cpu_tab_t *tab);
