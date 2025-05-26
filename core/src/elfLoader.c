#include "elfLoader.h"
#include "bits.h"
#include "core/addrSpace.h"
#include "core/kString.h"

// ELF magic bytes
static const u8_ct ELF_MAGIC[4] = {0x7F, 'E', 'L', 'F'};

size_ct elf_get_load_size(addr_ct elf_start)
{
    elf64_ehdr_ct *eh = (elf64_ehdr_ct *)elf_start;
    size_ct size = 0;
    for (u16_ct i = 0; i < eh->e_phnum; i++)
    {
        elf64_phdr_ct *ph = &((elf64_phdr_ct *)(elf_start + eh->e_phoff))[i];
        size_ct end_offset = ph->p_offset + ph->p_filesz;
        if (end_offset > size)
        {
            size = end_offset;
        }
    }

    return size;
}

err_code_ct elf_load(addr_ct elf_start, bool_ct is_kernel)
{
    u8_ct *data = (u8_ct *)elf_start;

    if (!is_kernel && kmemcmp(data, ELF_MAGIC, 4) != 0)
    {
        return BAD_PARAM;
    }

    u8_ct ei_data = data[5];
    u16_ct e_machine = ((elf64_ehdr_ct *)data)->e_machine;

    if (ELFDATA2LSB != ei_data || EM_X86_64 != e_machine)
    {
        return NOT_SUPPORTED;
    }

    u8_ct ei_class = data[4];

    if (ELFCLASS64 == ei_class)
    {
        elf64_ehdr_ct *ehdr = (elf64_ehdr_ct *)data;
        elf64_phdr_ct *phdrs = (elf64_phdr_ct *)(data + ehdr->e_phoff);

        for (u16_ct i = 0; i < ehdr->e_phnum; i++)
        {
            elf64_phdr_ct *ph = &phdrs[i];
            if (PT_LOAD != ph->p_type)
            {
                continue;
            }

            virt_mem_info_ct *mem_context = NULL;
            u32_ct mapping_flags = 0;
            if (ph->p_flags & PF_W)
            {
                mapping_flags |= PTE_READ_WRITE;
            }
            if (!(ph->p_flags & PF_X))
            {
                mapping_flags |= PTE_EXEC_DISABLED;
            }

            if (is_kernel)
            {
                mapping_flags |= FORCE_IF_ALREADY_MAPPED;
                mem_context = mem_get_kernel_mem_info();
            }
            else
            {
                mapping_flags |= PTE_USER;
            }

            err_code_ct map_ret = mem_map_to_phys_addr(
                mem_context, ALIGN_DOWN_TO_ADDR(ph->p_vaddr, PAGE_SIZE), ALIGN_DOWN_TO_ADDR(ph->p_paddr, PAGE_SIZE),
                ALIGN_UP_TO_VAL(ph->p_memsz, PAGE_SIZE) / PAGE_SIZE, mapping_flags);
            if (NO_ERROR != map_ret)
            {
                return map_ret;
            }

            // Zero out the rest if p_memsz > p_filesz
            if (ph->p_memsz > ph->p_filesz)
            {
                kmemset((u8_ct *)ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
            }
        }
    }
    else
    {
        // Unsupported class
        return NOT_SUPPORTED;
    }

    return NO_ERROR;
}