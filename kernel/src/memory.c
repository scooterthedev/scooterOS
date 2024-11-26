#include <kernel.h>

uint64_t paging_directory[512] __attribute__((aligned(4096)));
uint64_t paging_pml4[512] __attribute__((aligned(4096)));
uint64_t paging_pdpt[512] __attribute__((aligned(4096)));
uint64_t paging_pd[512] __attribute__((aligned(4096)));
uint64_t paging_pt[512] __attribute__((aligned(4096)));

void init_paging() {
    // Identity map lower 4GB

    // Initialize PML4
    paging_pml4[0] = ((uint64_t)paging_pdpt) | 0x3; // Present, Read/Write

    // Initialize PDPT
    paging_pdpt[0] = ((uint64_t)paging_pd) | 0x3; // Present, Read/Write

    // Initialize PD
    paging_pd[0] = ((uint64_t)paging_pt) | 0x3; // Present, Read/Write

    // Initialize PT
    for(int i = 0; i < 512; i++) {
        paging_pt[i] = (i * 0x1000) | 0x3; // Present, Read/Write
    }

    // Load CR3 with the address of the PDPT
    asm volatile("mov %0, %%cr3" : : "r"(paging_pdpt));

    // Enable paging and long mode
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set the PG bit (Paging)
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x20;        // Set PAE bit
    asm volatile("mov %0, %%cr4" : : "r"(cr4));

    // Enable long mode (EFER.LME)
    asm volatile(
        "mov $0xC0000080, %%ecx\n\t"  // EFER MSR address
        "rdmsr\n\t"
        "or $0x100, %%eax\n\t"         // Set LME bit
        "wrmsr\n\t"
        :
        :
        : "eax", "edx"
    );
}

void init_memory() {
    init_paging();
} 