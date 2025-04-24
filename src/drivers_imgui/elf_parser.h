#pragma once

int elf_parser_load_elf(char * filename);

bool elf_parser_adr2line(uint32_t adr, std::string &line);