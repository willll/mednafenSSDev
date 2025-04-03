#pragma once

typedef struct {
    char *name;
    uint32_t value;
    uint32_t size;
} debugui_cpu_reg_t;

typedef struct {
    debugui_cpu_reg_t * regs;
    size_t regs_count;
} debugui_cpu_tab_t;
