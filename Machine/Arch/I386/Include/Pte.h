/* Machine: pte.h,v 1.00 2022/06/19 */

#ifndef _I386_PTE_H_
#define _I386_PTE_H_

/*
 * i386 MMU hardware structure:
 *
 * the i386 MMU is a two-level MMU which maps 4GB of virtual memory.
 * the pagesize is 4K (4096 [0x1000] bytes), although newer pentium
 * processors can support a 4MB pagesize as well.
 *
 * the first level table (segment table?) is called a "page directory"
 * and it contains 1024 page directory entries (PDEs).   each PDE is
 * 4 bytes (an int), so a PD fits in a single 4K page.   this page is
 * the page directory page (PDP).  each PDE in a PDP maps 4MB of space 
 * (1024 * 4MB = 4GB).   a PDE contains the physical address of the 
 * second level table: the page table.   or, if 4MB pages are being used,
 * then the PDE contains the PA of the 4MB page being mapped.
 *
 * a page table consists of 1024 page table entries (PTEs).  each PTE is
 * 4 bytes (an int), so a page table also fits in a single 4K page.  a
 * 4K page being used as a page table is called a page table page (PTP).
 * each PTE in a PTP maps one 4K page (1024 * 4K = 4MB).   a PTE contains
 * the physical address of the page it maps and some flag bits (described
 * below).
 * 
 * the processor has a special register, "cr3", which points to the
 * the PDP which is currently controlling the mappings of the virtual
 * address space.
 *
 * the following picture shows the translation process for a 4K page:
 *
 * %cr3 register [PA of PDP]
 *      |
 *      |
 *      |   bits <31-22> of VA         bits <21-12> of VA   bits <11-0>
 *      |   index the PDP (0 - 1023)   index the PTP        are the page offset
 *      |         |                           |                  |
 *      |         v                           |                  |
 *      +--->+----------+                     |                  |
 *           | PD Page  |   PA of             v                  |
 *           |          |---PTP-------->+------------+           |
 *           | 1024 PDE |               | page table |--PTE--+   |
 *           | entries  |               | (aka PTP)  |       |   |
 *           +----------+               | 1024 PTE   |       |   |
 *                                      | entries    |       |   |
 *                                      +------------+       |   |
 *                                                           |   |
 *                                                bits <31-12>   bits <11-0>
 *                                                p h y s i c a l  a d d r
 *
 * the i386 caches PTEs in a TLB.   it is important to flush out old
 * TLB mappings when making a change to a mappings.   writing to the 
 * %cr3 will flush the entire TLB.    newer processors also have an
 * instruction that will invalidate the mapping of a single page (which
 * is useful if you are changing a single mappings because it preserves
 * all the cached TLB entries).
 *
 * as shows, bits 31-12 of the PTE contain PA of the page being mapped.
 * the rest of the PTE is defined as follows:
 *   bit#	name	use
 *   11		n/a	available for OS use, hardware ignores it
 *   10		n/a	available for OS use, hardware ignores it
 *   9		n/a	available for OS use, hardware ignores it
 *   8		G	global bit (see discussion below)
 *   7		PS	page size [for PDEs] (0=4k, 1=4M <if supported>) 
 *   6		D	dirty (modified) page
 *   5		A	accessed (referenced) page
 *   4		PCD	cache disable
 *   3		PWT	prevent write through (cache)
 *   2		U/S	user/supervisor bit (0=supervisor only, 1=both u&s)
 *   1		R/W	read/write bit (0=read only, 1=read-write)
 *   0		P	present (valid)
 *
 * notes: 
 *  - on the i386 the R/W bit is ignored if processor is in supervisor
 *    state (bug!)
 *  - PS is only supported on newer processors
 *  - PTEs with the G bit are global in the sense that they are not 
 *    flushed from the TLB when %cr3 is written (to flush, use the 
 *    "flush single page" instruction).   this is only supported on
 *    newer processors.    this bit can be used to keep the kernel's
 *    TLB entries around while context switching.   since the kernel
 *    is mapped into all processes at the same place it does not make 
 *    sense to flush these entries when switching from one process'
 *    pmap to another.
 */

#if !defined(_LOCORE)

/*
 * here we define the data types for PDEs and PTEs
 */

typedef u_int32_t pd_entry_t;		/* PDE */
typedef u_int32_t pt_entry_t;		/* PTE */

#endif

/*
 * now we define various for playing with virtual addresses
 */

#define	PDSHIFT		22		/* offset of PD index in VA */
#define	NBPD		(1 << PDSHIFT)	/* # bytes mapped by PD (4MB) */
#define	PDOFSET		(NBPD-1)	/* mask for non-PD part of VA */
#if 0 /* not used? */
#define	NPTEPD		(NBPD / NBPG)	/* # of PTEs in a PD */
#else
#define	PTES_PER_PTP	(NBPD / NBPG)	/* # of PTEs in a PTP */
#endif
#define	PD_MASK		0xffc00000	/* page directory address bits */
#define	PT_MASK		0x003ff000	/* page table address bits */

/*
 * here we define the bits of the PDE/PTE, as described above:
 *
 * XXXCDC: need to rename these (PG_u == ugly).
 */

#define	PG_V		0x00000001	/* valid entry */
#define	PG_RO		0x00000000	/* read-only page */
#define	PG_RW		0x00000002	/* read-write page */
#define	PG_u		0x00000004	/* user accessible page */
#define	PG_PROT		0x00000006	/* all protection bits */
#define	PG_N		0x00000018	/* non-cacheable */
#define	PG_U		0x00000020	/* has been used */
#define	PG_M		0x00000040	/* has been modified */
#define PG_PS		0x00000080	/* 4MB page size */
#define PG_G		0x00000100	/* global, don't TLB flush */
#define PG_AVAIL1	0x00000200	/* ignored by hardware */
#define PG_AVAIL2	0x00000400	/* ignored by hardware */
#define PG_AVAIL3	0x00000800	/* ignored by hardware */
#define	PG_FRAME	0xfffff000	/* page frame mask */

/*
 * various short-hand protection codes
 */

#define	PG_KR		0x00000000	/* kernel read-only */
#define	PG_KW		0x00000002	/* kernel read-write */

/*
 * page protection exception bits
 */

#define PGEX_P		0x01	/* protection violation (vs. no mapping) */
#define PGEX_W		0x02	/* exception during a write cycle */
#define PGEX_U		0x04	/* exception while in user mode (upl) */

#endif /* _I386_PTE_H_ */