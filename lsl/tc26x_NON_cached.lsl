//****************************************************************************
//**                                                                         *
//**  FILE        :  tc26x.lsl   (modified TC27x.lsl for the example code)   *
//**                                                                         *
//**  DESCRIPTION :  LSL description: Infineon TC27X                         *
//**                                                                         *
//**  Copyright 2011-2012 Altium BV                                         *
//**                                                                         *
//****************************************************************************

#include "tc1v1_6_x.lsl"

#define HAS_ON_CHIP_FLASH       // this derivative has on-chip flash
#define HAS_NO_EXTERNAL_RAM     // Needed for template.lsl and extmem.lsl to exclude xram_8_a

#ifndef    __REDEFINE_ON_CHIP_ITEMS

// Specify a multi-core processor environment (mpe)

processor mpe
{
        derivative = tc26x;
}
#endif  // __REDEFINE_ON_CHIP_ITEMS

#ifndef CSA_TC0
#define CSA_TC0         64                      /* number of context blocks tc0 */
#endif

#ifndef CSA_TC1
#define CSA_TC1         64                      /* number of context blocks tc1 */
#endif


#ifndef CSA_START_TC0
#define CSA_START_TC0   0xd0004000              /* start address of CSA tc0 */
#endif

#ifndef CSA_START_TC1
#define CSA_START_TC1   0xd0004000              /* start address of CSA tc1 */
#endif



#ifndef USTACK_TC0
#define USTACK_TC0      16k                     /* user stack size tc0 */
#endif

#ifndef USTACK_TC1
#define USTACK_TC1      1k                      /* user stack size tc1 */
#endif



#ifndef ISTACK_TC0
#define ISTACK_TC0      1k                      /* interrupt stack size tc0 */
#endif

#ifndef ISTACK_TC1
#define ISTACK_TC1      1k                      /* interrupt stack size tc1 */
#endif


#ifndef XVWBUF
#define XVWBUF          0                       /* buffer used by debugger */
#endif

#ifndef INTTAB
#define INTTAB          0xa00f0000              /* start address of interrupt table */
#endif
#ifndef TRAPTAB
#define TRAPTAB         (INTTAB + 0x6000)      /* start address of trap table */
#endif

#ifndef INTTAB0
#define INTTAB0         (INTTAB)
#endif
#ifndef INTTAB1
#define INTTAB1         (INTTAB0 + 0x2000)
#endif


#ifndef TRAPTAB0
#define TRAPTAB0        (TRAPTAB)
#endif
#ifndef TRAPTAB1
#define TRAPTAB1        (TRAPTAB0 + 0x100)
#endif


#ifndef RESET
#define RESET           0xa0000020              /* internal flash start address tc0 */
#endif
/***********************************/
/* Because ECLIPSE is generating Ax_START instead of Ax_START_ADDRESS */
/***********************************/
#ifndef A0_START_ADDRESS
#ifdef  A0_START
#define A0_START_ADDRESS A0_START
#endif
#endif
#ifndef A1_START_ADDRESS
#ifdef  A1_START
#define A1_START_ADDRESS A1_START
#endif
#endif
#ifndef A8_START_ADDRESS
#ifdef  A8_START
#define A8_START_ADDRESS A8_START
#endif
#endif
#ifndef A9_START_ADDRESS
#ifdef  A9_START
#define A9_START_ADDRESS A9_START
#endif
#endif
/***********************************/

derivative tc26x
{
        core tc0 // core 0 TC16E
        {
                architecture = TC1V1.6.X;
                space_id_offset = 100;          // add 100 to all space IDs in the architecture definition
                copytable_space = vtc:linear;     // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core tc1 // core 1 TC16E
        {
                architecture = TC1V1.6.X;
                space_id_offset = 200;          // add 200 to all space IDs in the architecture definition
                copytable_space = vtc:linear;     // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core mcs00 // core MCS 00
        {
                architecture = MCS;
                copytable_space = vtc:linear;   // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core mcs01 // core MCS 01
        {
                architecture = MCS;
                copytable_space = vtc:linear;   // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core mcs02 // core MCS 02
        {
                architecture = MCS;
                copytable_space = vtc:linear;   // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core mcs03 // core MCS 03
        {
                architecture = MCS;
                copytable_space = vtc:linear;   // use the copy table in the virtual core for 'bss' and initialized data sections
        }
        
        core vtc
        {
                architecture = TC1V1.6.X;
                import tc0;                     // add all address spaces of core tc0 to core vtc for linking and locating
                import tc1;                     //                                tc1
        }

        bus sri
        {
                mau = 8;
                width = 32;

                // map shared addresses one-to-one to real cores and virtual cores
                map (dest=bus:tc0:fpi_bus, src_offset=0, dest_offset=0, size=0xc0000000);
                map (dest=bus:tc1:fpi_bus, src_offset=0, dest_offset=0, size=0xc0000000);
                map (dest=bus:vtc:fpi_bus, src_offset=0, dest_offset=0, size=0xc0000000);

                // map shared addresses one-to-one to real cores and virtual 
                map (dest=bus:tc0:fpi_bus, src_offset=0xe0000000, dest_offset=0xe0000000, size=0x20000000);
                map (dest=bus:tc1:fpi_bus, src_offset=0xe0000000, dest_offset=0xe0000000, size=0x20000000);
                map (dest=bus:vtc:fpi_bus, src_offset=0xe0000000, dest_offset=0xe0000000, size=0x20000000);
        }

        bus aei
        {
                mau = 8;
                width = 32;
                map (dest=bus:sri, dest_offset=0, size=0x100000000);
        }

#ifndef    __REDEFINE_ON_CHIP_ITEMS

        memory dspr0 // Data Scratch Pad Ram
        {
                mau = 8;
                size = 72k;
                type = ram;
                map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=72k, priority=8);
                map (dest=bus:sri, dest_offset=0x70000000, size=72k);
        }

        memory pspr0 // Program Scratch Pad Ram
        {
                mau = 8;
                size = 16k;
                type = ram;
                map (dest=bus:tc0:fpi_bus, dest_offset=0xc0000000, size=16k, priority=8);
                map (dest=bus:sri, dest_offset=0x70100000, size=16k);
        }

        memory dspr1 // Data Scratch Pad Ram
        {
                mau = 8;
                size = 120k;
                type = reserved ram;
                map (dest=bus:tc1:fpi_bus, dest_offset=0xd0000000, size=120k, priority=8);
                map (dest=bus:sri, dest_offset=0x60000000, size=120k);
        }

        memory pspr1 // Program Scratch Pad Ram
        {
                mau = 8;
                size = 24k;
                type = reserved ram;
                map (dest=bus:tc1:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
                map (dest=bus:sri, dest_offset=0x60100000, size=24k);
        }

        memory pflash0
        {
                mau = 8;
                size = 1M;
                type = rom;
                map     cached (dest=bus:sri, dest_offset=0x80000000, reserved, size=1M);
                map not_cached (dest=bus:sri, dest_offset=0xa0000000,           size=1M);
        }

        memory pflash1
        {
                mau = 8;
                size = 1536k;
                type = rom;
                map     cached (dest=bus:sri, dest_offset=0x80100000, reserved, size=1536k);
                map not_cached (dest=bus:sri, dest_offset=0xa0100000,           size=1536k);
        }

        memory brom
        {
                mau = 8;
                size = 32k;
                type = reserved rom;
                map     cached (dest=bus:sri, dest_offset=0x8fff8000,           size=32k);
                map not_cached (dest=bus:sri, dest_offset=0xafff8000, reserved, size=32k);
        }

        memory dflash0
        {
                mau = 8;
                size = 1m+16k;
                type = reserved nvram;
                map (dest=bus:sri, dest_offset=0xaf000000, size=1m  );
                map (dest=bus:sri, dest_offset=0xaf100000, size=16k );
        }

#endif  // __REDEFINE_ON_CHIP_ITEMS

        section_setup :vtc:linear
        {
#ifndef __LINKONLY__
                start_address
                (
                        run_addr = (RESET),
                        symbol = "_START"
                );
#endif
        }

        section_setup :vtc:linear
        {
                stack "ustack_tc0"
                (
                        min_size = (USTACK_TC0),
                        fixed,
                        align = 8
                );

                stack "ustack_tc1"
                (
                        min_size = (USTACK_TC1),
                        fixed,
                        align = 8
                );

                stack "istack_tc0"
                (
                        min_size = (ISTACK_TC0),
                        fixed,
                        align = 8
                );

                stack "istack_tc1"
                (
                        min_size = (ISTACK_TC1),
                        fixed,
                        align = 8
                );
        }

        section_layout :vtc:linear
        {
                "_lc_ue_ustack" := "_lc_ue_ustack_tc0"; /* common cstart interface for first or single core */
                "_lc_ue_istack" := "_lc_ue_istack_tc0"; /* common cstart interface for first or single core */
        }

        section_setup :vtc:linear
        {
                copytable
                (
                        align = 4,
                        dest = linear,
                        table
                        {
                                symbol = "_lc_ub_table_tc1";
                                space = :tc1:linear, :tc1:abs24, :tc1:abs18, :tc1:csa;
                        }
                );
        }
        
        section_layout :tc0:csa
        {
                group  (ordered, align = 64, attributes=rw, run_addr=(CSA_START_TC0)) 
                        reserved "csa_tc0" (size = 64 * (CSA_TC0));
                "_lc_ub_csa_01" := "_lc_ub_csa_tc0"; /* common cstart interface for first or single core */
                "_lc_ue_csa_01" := "_lc_ue_csa_tc0"; /* common cstart interface for first or single core */
        }

        section_layout :tc1:csa
        {
                group  (ordered, align = 64, attributes=rw, run_addr=(CSA_START_TC1)) 
                        reserved "csa_tc1" (size = 64 * (CSA_TC1));
        }


        section_layout :vtc:linear
        {
#if (XVWBUF>0)
                        group (align = 4) reserved "_xvwbuffer_" (size=XVWBUF, attributes=rw);
                        "_lc_ub_xvwbuffer" = "_lc_ub__xvwbuffer_";
                        "_lc_ue_xvwbuffer" = "_lc_ue__xvwbuffer_";
#endif

                        // quasi address spaces (relative to register)
                        
#ifdef A0_START_ADDRESS
                        group a0 (ordered, contiguous, run_addr=(A0_START_ADDRESS))
#else
                        group a0 (ordered, contiguous)
#endif
                        {
                                select "(.sdata|.sdata.*)";
                                select "(.sbss|.sbss.*)";
                        }
                        "_SMALL_DATA_" := sizeof(group:a0) > 0 ? addressof(group:a0) + 0x8000 : 0;
                        "_SMALL_DATA_TC0" := "_SMALL_DATA_";
                        "_SMALL_DATA_TC1" := "_SMALL_DATA_";

#ifdef A1_START_ADDRESS
                        group a1 (ordered, contiguous, run_addr=(A1_START_ADDRESS))
#else
                        group a1 (ordered, contiguous)
#endif
                        {
                                select "(.ldata|.ldata.*)";
                        }
                        "_LITERAL_DATA_" := sizeof(group:a1) > 0 ? addressof(group:a1) + 0x8000 : 0;
                        "_LITERAL_DATA_TC0" := "_LITERAL_DATA_";
                        "_LITERAL_DATA_TC1" := "_LITERAL_DATA_";
                        
#ifdef A8_START_ADDRESS
                        group a8 (ordered, contiguous, run_addr=(A8_START_ADDRESS))
#else
                        group a8 (ordered, contiguous)
#endif
                        {
                                select "(.data_a8|.data_a8.*)";
                                select "(.bss_a8|.bss_a8.*)";
                                select "(.rodata_a8|.rodata_a8.*)";
                        }
                        "_A8_DATA_" := sizeof(group:a8) > 0 ? addressof(group:a8) + 0x8000 : 0;
                        "_A8_DATA_TC0" := "_A8_DATA_";
                        "_A8_DATA_TC1" := "_A8_DATA_";

#ifdef A9_START_ADDRESS
                        group a9 (ordered, contiguous, run_addr=(A9_START_ADDRESS))
#else
                        group a9 (ordered, contiguous)
#endif
                        {
                                select "(.data_a9|.data_a9.*)";
                                select "(.bss_a9|.bss_a9.*)";
                                select "(.rodata_a9|.rodata_a9.*)";
                        }
                        "_A9_DATA_" := sizeof(group:a9) > 0 ? addressof(group:a9) + 0x8000 : 0;
                        "_A9_DATA_TC0" := "_A9_DATA_";
                        "_A9_DATA_TC1" := "_A9_DATA_";
        }

        section_layout :vtc:linear
        {
                        "_lc_u_int_tab_tc0" = (INTTAB0);
                        "_lc_u_int_tab_tc1" = (INTTAB1);
                        "_lc_u_int_tab" = "_lc_u_int_tab_tc0"; /* common cstart interface for first or single core */

                        // interrupt vector tables for tc0, tc1, tc2
                        group int_tab_tc0 (ordered)
                        {
#                               include "inttab0.lsl"
                        }
                        group int_tab_tc1 (ordered)
                        {
#                               include "inttab1.lsl"
                        }

                        "_lc_u_trap_tab_tc0" = (TRAPTAB0);
                        "_lc_u_trap_tab_tc1" = (TRAPTAB1);
                        "_lc_u_trap_tab" = "_lc_u_trap_tab_tc0"; /* common cstart interface for first or single core */

                        // trapvector tables for tc0, tc1, tc2
                        group trap_tab_tc0 (ordered)
                        {
#                               include "traptab0.lsl"           
                        }
                        group trap_tab_tc1 (ordered)
                        {
#                               include "traptab1.lsl"           
                        }

        }

#ifndef    __REDEFINE_ON_CHIP_ITEMS

#include        "memdef_04_mcs.lsl"

#endif  // __REDEFINE_ON_CHIP_ITEMS

}

