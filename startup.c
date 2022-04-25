/**
 * @file startup.c
 *
 * @date 23.06.2019
 * @author twatorowski (tw@mightydevices.com)
 *
 * @brief stm32l433 startup file
 */

#include <stdint.h>
#include <stddef.h>

#include "linker.h"
#include "compiler.h"

/* main program routine */
extern int Main(void);

/* copy a memory section */
static void Startup_CopySection(void *dst, const void *src, size_t size)
{
    /* construct pointers */
    uint8_t *d = dst; const uint8_t *s = src;

    /* some unrolling */
    for (; size >= 4; size -= 4)
        *d++ = *s++, *d++ = *s++, *d++ = *s++, *d++ = *s++;

    /* final touch */
    switch (size) {
    case 3 : *d++ = *s++;
    case 2 : *d++ = *s++;
    case 1 : *d++ = *s++;
    }
}

/* clear a memory section */
static void Startup_ZeroSection(void *ptr, size_t size)
{
    /* construct pointer */
    uint8_t *p = ptr;

    /* some unrolling */
    for (; size >= 4; size -= 4)
        *p++ = 0, *p++ = 0, *p++ = 0, *p++ = 0;

    /* final touch */
    switch (size) {
    case 3 : *p++ = 0;
    case 2 : *p++ = 0;
    case 1 : *p++ = 0;
    }
}

void StartHere(void)
{
  asm volatile(
	  "LDR.W   R0, =0xE000ED88\n\t"       // CPACR is located at address 0xE000ED88
	  "LDR     R1, [R0]\n\t"              // Read CPACR
	  "ORR     R1, R1, #(0xF << 20)\n\t"  // Set bits 20-23 to enable CP10 and CP11 coprocessors
	  "STR     R1, [R0]\n\t"              // Write back the modified value to the CPACR
	  "DSB\n\t"                           // Wait for store to complete
	  "ISB\n\t"                           // Reset pipeline now the FPU is enabled
  );
}


/* first function to be executed after the reset. Shall initialize the chip
 * to it's default state */
void SECTION(".flash_code") Startup_ResetHandler(void)
{
    /* initialize ram functions */
    Startup_CopySection(&__ram_code_addr, &__flash_sram_init_src_addr, 
        (size_t)&__ram_code_size);
    /* initialize data */
    Startup_CopySection(&__data_addr,  (uint8_t *)&__flash_sram_init_src_addr + 
        (size_t)&__ram_code_size, (size_t)&__data_size);
    /* zero out the bss */
	Startup_ZeroSection(&__bss_addr, (size_t)&__bss_size);
	
	StartHere();
	/* jump to main program routine */
	Main();
}
