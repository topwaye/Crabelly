#ifndef I386_PTE_MACHINE
#define I386_PTE_MACHINE

#define	PDSHIFT		22		/* LOG2(NBPDR) */
#define	NBPD		  (1 << PDSHIFT)	/* bytes/page dir */
#define	PDOFSET		(NBPD-1)	/* byte offset into page dir */
#define	NPTEPD		(NBPD / NBPG)

#ifndef _LOCORE
typedef unsigned long	pd_entry_t;		/* page directory entry */
typedef unsigned long	pt_entry_t;		/* Mach page table entry */
#endif

#define	PD_MASK		0xffc00000	/* page directory address bits */
#define	PT_MASK		0x003ff000	/* page table address bits */

#define	PG_V		  0x00000001	/* present */
#define	PG_RO		  0x00000000	/* read-only by user (and kernel if 486) */
#define	PG_RW		  0x00000002	/* read-write by user */
#define	PG_u		  0x00000004	/* accessible by user */
#define	PG_PROT		  0x00000006	/* all protection bits */
#define	PG_N		  0x00000018	/* non-cacheable */
#define	PG_U		  0x00000020	/* has been used */
#define	PG_M		  0x00000040	/* has been modified */
#define	PG_W		  0x00000200	/* page is wired */
#define	PG_FRAME	0xfffff000	/* page frame mask */

#define	PG_FZERO	0
#define	PG_FTEXT	1
#define	PG_FMAX		(PG_FTEXT)

#define	PG_NOACC	0
#define	PG_KR		  0x00000000
#define	PG_KW		  0x00000002
#define	PG_URKR		0x00000004
#define	PG_URKW		0x00000004
#define	PG_UW		  0x00000006

/*
 * Page Protection Exception bits
 */
#define PGEX_P		0x01	/* Protection violation vs. not present */
#define PGEX_W		0x02	/* during a Write cycle */
#define PGEX_U		0x04	/* access from User mode (UPL) */

#ifndef _LOCORE
#ifdef _KERNEL
/* utilities defined in pmap.c */
extern	pt_entry_t *Sysmap;
#endif
#endif

#endif /* I386_PTE_MACHINE */
