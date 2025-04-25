#include <iostream>
#include <string>
#include <cstdlib>

#ifdef ELF_BFD
// fix for mingw
#undef PACKAGE
#undef PACKAGE_VERSION
#define PACKAGE "mingw_fix_bfd"
#define PACKAGE_VERSION 0
#include <bfd.h>
struct ElfParser
{
    bfd *abfd;
    asymbol **syms;
    long symcount;

    ElfParser() : abfd(nullptr), syms(nullptr), symcount(0) {}

    ~ElfParser()
    {
        if (abfd)
        {
            bfd_close(abfd);
        }
        if (syms)
        {
            free(syms);
        }
    }
};

// Déclaration de l'instance statique de ElfParser
static ElfParser parser;

int elf_parser_load_elf(char *filename)
{
    bfd_init();
    parser.abfd = bfd_openr(filename, nullptr);

    if (!parser.abfd)
    {
        printf("Failed to open %s\n", filename);
        return -1;
    }

    if (!bfd_check_format(parser.abfd, bfd_object))
    {
        printf("%s is not an object file\n", filename);
        bfd_close(parser.abfd);
        parser.abfd = nullptr;
        return -1;
    }

    long storage = bfd_get_symtab_upper_bound(parser.abfd);
    if (storage < 0)
    {
        printf("no debug info %s\n", filename);
        bfd_close(parser.abfd);
        parser.abfd = nullptr;
        return -1;
    }

    parser.syms = (asymbol **)malloc(storage);
    parser.symcount = bfd_canonicalize_symtab(parser.abfd, parser.syms);

    if (parser.symcount <= 0)
    {
        printf("failed to read debug info %s\n", filename);
        free(parser.syms);
        parser.syms = nullptr;
        bfd_close(parser.abfd);
        parser.abfd = nullptr;
        return -1;
    }

    return true;
}

/* Look for an address in a section.  This is called via
   bfd_map_over_sections.  */
static bfd_vma pc;
static const char *filename;
static const char *functionname;
static unsigned int line;
static bool found;
static void /*__attribute__((optimize("O0")))*/
find_address_in_section(bfd *abfd, asection *section, void *data)

{
    bfd_vma vma;

    if (found)
        return;

    if ((bfd_section_flags(section) & SEC_ALLOC) == 0)
        return;

    vma = bfd_section_vma( section);
    if (pc < vma)
        return;

    found = bfd_find_nearest_line(parser.abfd, section, parser.syms, pc - vma,
                                  &filename, &functionname, &line);
}

bool /*__attribute__((optimize("O0")))*/ elf_parser_adr2line(uint32_t adr, std::string &str)
{
    // no elf loaded
    if (!parser.abfd)
    {
        return false;
    }

    found = false;
    pc = adr;
    bfd_map_over_sections (parser.abfd, find_address_in_section, nullptr);


    if (found)
    {
        str = std::string(filename ? filename : "Unknown file") + ":" + std::to_string(line);
        if (functionname)
        {
            str += " in function " + std::string(functionname);
        }
    }

    return found;
}
#else
int elf_parser_load_elf(const char *filename) { return -1; }
bool elf_parser_adr2line(uint32_t adr, std::string &line) { return false; }
#endif