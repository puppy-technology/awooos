#include <dmm.h>
#include "magic.h"
#include "multiboot.h"
#include "../src/panic.h"
#include <stddef.h>
#include <stdio.h>

#define MBOOT_FLAG_A_OUT 4
#define MBOOT_FLAG_ELF   5
#define MBOOT_FLAG_MMAP  6

extern size_t kernel_start;
extern size_t kernel_end;

static size_t hal_kernel_start = (size_t)&kernel_start;
static size_t hal_kernel_end   = (size_t)(&kernel_end) + 1;

void multiboot_validate_info(MultibootInfo *multiboot_info)
{
#define multiboot_flag_set(bit) ((multiboot_info->flags & (1 << bit)) != 0)

    // Multiboot a.out and ELF flags should never be set simultaneously.
    // These should never be set at the same time.
    if (multiboot_flag_set(MBOOT_FLAG_A_OUT)
            && multiboot_flag_set(MBOOT_FLAG_ELF)) {
        hal_panic("invalid multiboot header: can't have both a.out and ELF.");
    }

    // We need ELF, not a.out.
    if (multiboot_flag_set(MBOOT_FLAG_A_OUT)) {
        hal_panic("invalid multiboot header: expected ELF section, got a.out.");
    }

    // We need ELF section header information to parse STAB information.
    if (!multiboot_flag_set(MBOOT_FLAG_ELF)) {
        hal_panic("invalid multiboot header: no ELF section provided.");
    }

    // We expect a memory map to initialize DMM.
    if (!multiboot_flag_set(MBOOT_FLAG_MMAP)) {
        hal_panic("invalid multiboot header: no memory map provided.");
    }
#undef multiboot_flag_set
}

void multiboot_add_mmap_entry(MultibootMemoryMapEntry *mmap_entry)
{
    if (mmap_entry->type != MULTIBOOT_MEMORY_MAP_AVAILABLE) {
        puts("        Memory map entry is for an unusable region.");
        return;
    }

    // If we get an entry starting at 0x0, make it instead start at
    // 0x1 and decrease it's length by 1 byte.
    if ((size_t)mmap_entry->addr == 0) {
        puts("        Got entry for 0x0 -- starting at 0x1 instead.");
        mmap_entry->addr += 1;
        mmap_entry->length -= 1;
    }

    // If we get an entry that contains the kernel, only use the portion
    // after the kernel.
    //
    // QUESTIONABLE ASSUMPTION: The portion before the kernel won't be useful.
    //
    // An alternative approach would be to split it into two
    // entries -- e.g.,
    //     <start of actual entry> through (hal_kernel_start - 1)
    //     (hal_kernel_end + 1) through <end of actual entry>

    // If the entry contains nothing but the kernel, just skip it.
    if ((mmap_entry->addr >= (uint64_t)hal_kernel_start) &&
            ((mmap_entry->addr + mmap_entry->size) <= (uint64_t)hal_kernel_end)) {
        puts("        Memory map entry contains nothing but the kernel; skipping.");
        return;
    }

    // If the entry starts before the kernel but ends after the start of
    // the kernel, take the part before the kernel.
    //
    // This is set up so that the part after the kernel, if any,
    // will be added later in the function.
    if ((mmap_entry->addr < (uint64_t)hal_kernel_start) &&
            ((mmap_entry->addr + mmap_entry->size) >= (uint64_t)hal_kernel_start)
        ) {
        puts("        Memory map entry runs into kernel -- adding part before kernel.");

        uint64_t adjustment = (mmap_entry->addr - hal_kernel_start);

        // Add the region before the kernel.
        dmm_add_memory_region((void*)mmap_entry->addr, (size_t)(mmap_entry->size - adjustment));

        // Adjust the mmap entry to point to the start of the kernel.
        mmap_entry->addr += adjustment;
        mmap_entry->size -= adjustment;
    }

    // If the the entry starts inside the kernel, adjust it to be after
    // the kernel.
    if ((mmap_entry->addr >= (uint64_t)hal_kernel_start) ||
            (mmap_entry->addr <= (uint64_t)hal_kernel_end)) {
        puts("        Memory map entry overlaps end of kernel -- starting after kernel.");

        mmap_entry->size -= (hal_kernel_end - mmap_entry->addr);
        mmap_entry->addr = hal_kernel_end;
    }

    // If we get this far, add the memory region to DMM.
    //
    // ASSUMPTION: Despite Multiboot using uint64_t for length, assume
    //             it will always fit in a size_t.
    puts("        Adding memory map entry to DMM.");
    dmm_add_memory_region((void*)mmap_entry->addr, (size_t)mmap_entry->length);
}

void hal_multiboot_init()
{
    // Get the Multiboot info struct.
    MultibootInfo *multiboot_info = ((MultibootInfo*)hal_get_arg());

    puts("    Validating multiboot information.");
    multiboot_validate_info(multiboot_info);

    puts("    Doing questionable pointer arithmetic.");
    // Initialize DMM with available memory regions (as told by multiboot).
    size_t mmap_addr = multiboot_info->mmap_addr;
    size_t mmap_count = ((size_t)multiboot_info->mmap_length) / sizeof(MultibootMemoryMapEntry);
    MultibootMemoryMapEntry *mmap_entries = (MultibootMemoryMapEntry*)mmap_addr;

    puts("      Adding memory map entries.");
    for (size_t i = 0; i < mmap_count; i++) {
        multiboot_add_mmap_entry(mmap_entries + i);
    }
    puts("      Done adding memory map entries.");
}
