#ifndef	PMAP_MACHINE
#define	PMAP_MACHINE

/*
 * see pte.h for a description of i386 MMU terminology and hardware
 * interface.
 *
 * a pmap describes a processes' 4GB virtual address space.  this
 * virtual address space can be broken up into 1024 4MB regions which
 * are described by PDEs in the PDP.   the PDEs are defined as follows:
 *
 * (ranges are inclusive -> exclusive, just like vm_map_entry start/end)
 * (the following assumes that KERNBASE is 0xf0000000)
 *
 * PDE#s	VA range		usage
 * 0->959	0x0 -> 0xefc00000	user address space, note that the
 *					max user address is 0xefbfe000
 *					the final two pages in the last 4MB
 *					used to be reserved for the UAREA
 *					but now are no longer used
 * 959		0xefc00000->		recursive mapping of PDP (used for
 *			0xf0000000	linear mapping of PTPs)
 * 960->1023	0xf0000000->		kernel address space (constant
 *			0xffc00000	across all pmap's/processes)
 * 1023		0xffc00000->		"alternate" recursive PDP mapping
 *			<end>		(for other pmaps)
 *
 *
 * note: a recursive PDP mapping provides a way to map all the PTEs for
 * a 4GB address space into a linear chunk of virtual memory.   in other
 * words, the PTE for page 0 is the first int mapped into the 4MB recursive 
 * area.   the PTE for page 1 is the second int.    the very last int in the
 * 4MB range is the PTE that maps VA 0xffffe000 (the last page in a 4GB
 * address).
 *
 * all pmap's PD's must have the same values in slots 960->1023 so that
 * the kernel is always mapped in every process.   these values are loaded
 * into the PD at pmap creation time.
 *
 * at any one time only one pmap can be active on a processor.   this is
 * the pmap whose PDP is pointed to by processor register %cr3.   this pmap
 * will have all its PTEs mapped into memory at the recursive mapping
 * point (slot #959 as show above).   when the pmap code wants to find the
 * PTE for a virtual address, all it has to do is the following:
 *
 * address of PTE = (959 * 4MB) + (VA / NBPG) * sizeof(pt_entry_t)
 *                = 0xefc00000 + (VA /4096) * 4
 *
 * what happens if the pmap layer is asked to perform an operation
 * on a pmap that is not the one which is currently active?   in that
 * case we take the PA of the PDP of non-active pmap and put it in 
 * slot 1023 of the active pmap.   this causes the non-active pmap's 
 * PTEs to get mapped in the final 4MB of the 4GB address space
 * (e.g. starting at 0xffc00000).
 *
 * the following figure shows the effects of the recursive PDP mapping:
 *
 *   PDP (%cr3)
 *   +----+
 *   |   0| -> PTP#0 that maps VA 0x0 -> 0x400000
 *   |    |
 *   |    |
 *   | 959| -> points back to PDP (%cr3) mapping VA 0xefc00000 -> 0xf0000000
 *   | 960| -> first kernel PTP (maps 0xf0000000 -> 0xf0400000)
 *   |    |
 *   |1023| -> points to alternate pmap's PDP (maps 0xffc00000 -> end)
 *   +----+
 *
 * note that the PDE#959 VA (0xefc00000) is defined as "PTE_BASE"
 * note that the PDE#1023 VA (0xffc00000) is defined as "APTE_BASE"
 *
 * starting at VA 0xefc00000 the current active PDP (%cr3) acts as a
 * PTP:
 *
 * PTP#959 == PDP(%cr3) => maps VA 0xefc00000 -> 0xf0000000
 *   +----+
 *   |   0| -> maps the contents of PTP#0 at VA 0xefc00000->0xefc01000
 *   |    |
 *   |    |
 *   | 959| -> maps contents of PTP#959 (the PDP) at VA 0xeffbf000
 *   | 960| -> maps contents of first kernel PTP 
 *   |    |
 *   |1023|
 *   +----+
 *
 * note that mapping of the PDP at PTP#959's VA (0xeffbf000) is 
 * defined as "PDP_BASE".... within that mapping there are two
 * defines: 
 *   "PDP_PDE" (0xeffbfefc) is the VA of the PDE in the PDP
 *      which points back to itself.     
 *   "APDP_PDE" (0xeffbfffc) is the VA of the PDE in the PDP which
 *      establishes the recursive mapping of the alternate pmap.
 *      to set the alternate PDP, one just has to put the correct
 *	PA info in *APDP_PDE.
 *
 * note that in the APTE_BASE space, the APDP appears at VA 
 * "APDP_BASE" (0xfffff000).
 */

/*
 * the following defines identify the slots used as described above.
 */

#define PDSLOT_PTE	((KERNBASE/NBPD)-1) /* 959: for recursive PDP map */
#define PDSLOT_KERN	(KERNBASE/NBPD)	    /* 960: start of kernel space */
#define PDSLOT_APTE	((unsigned)1023) /* 1023: alternative recursive slot */

/*
 * the following defines give the virtual addresses of various MMU 
 * data structures:
 * PTE_BASE and APTE_BASE: the base VA of the linear PTE mappings
 * PTD_BASE and APTD_BASE: the base VA of the recursive mapping of the PTD
 * PDP_PDE and APDP_PDE: the VA of the PDE that points back to the PDP/APDP
 */

#define PTE_BASE	((pt_entry_t *)  (PDSLOT_PTE * NBPD) )
#define APTE_BASE	((pt_entry_t *)  (PDSLOT_APTE * NBPD) )
#define PDP_BASE ((pd_entry_t *)  (((char *)PTE_BASE)  + (PDSLOT_PTE * NBPG)) )
#define APDP_BASE ((pd_entry_t *)  (((char *)APTE_BASE)  + (PDSLOT_APTE * NBPG)) )
#define PDP_PDE		(PDP_BASE + PDSLOT_PTE)
#define APDP_PDE	(PDP_BASE + PDSLOT_APTE)

/*
 * XXXCDC: tmp xlate from old names:
 * PTDPTDI -> PDSLOT_PTE
 * KPTDI -> PDSLOT_KERN
 * APTDPTDI -> PDSLOT_APTE
 */

/*
 * the follow define determines how many PTPs should be set up for the
 * kernel by locore.s at boot time.   this should be large enough to
 * get the VM system running.   once the VM system is running, the 
 * pmap module can add more PTPs to the kernel area on demand.
 */

#ifndef NKPTP
#define NKPTP		4	/* 16MB to start */
#endif
#define NKPTP_MIN	4	/* smallest value we allow */
#define NKPTP_MAX	(1024 - (KERNBASE/NBPD) - 1)
				/* largest value (-1 for APTP space) */

/*
 * various address macros
 *
 *  vtopte: return a pointer to the PTE mapping a VA
 *  kvtopte: same as above (takes a KVA, but doesn't matter with this pmap)
 *  ptetov: given a pointer to a PTE, return the VA that it maps
 *  vtophys: translate a VA to the PA mapped to it
 *
 * plus alternative versions of the above
 */

#define vtopte(VA)	(PTE_BASE + i386_btop(VA))
#define kvtopte(VA)	vtopte(VA)
#define ptetov(PT)	(i386_ptob(PT - PTE_BASE))
#define	vtophys(VA) ((*vtopte(VA) & PG_FRAME) | ((unsigned)(VA) & ~PG_FRAME))
#define	avtopte(VA)	(APTE_BASE + i386_btop(VA))
#define	ptetoav(PT)	(i386_ptob(PT - APTE_BASE)) 
#define	avtophys(VA) ((*avtopte(VA) & PG_FRAME) | ((unsigned)(VA) & ~PG_FRAME))

/*
 * pdei/ptei: generate index into PDP/PTP from a VA
 */
#define	pdei(VA)	(((VA) & PD_MASK) >> PDSHIFT)
#define	ptei(VA)	(((VA) & PT_MASK) >> PGSHIFT)

/*
 * PTP macros:
 *   a PTP's index is the PD index of the PDE that points to it
 *   a PTP's offset is the byte-offset in the PTE space that this PTP is at
 *   a PTP's VA is the first VA mapped by that PTP
 *
 * note that NBPG == number of bytes in a PTP (4096 bytes == 1024 entries)
 *           NBPD == number of bytes a PTP can map (4MB)
 */

#define ptp_i2o(I)	((I) * NBPG)	/* index => offset */
#define ptp_o2i(O)	((O) / NBPG)	/* offset => index */
#define ptp_i2v(I)	((I) * NBPD)	/* index => VA */
#define ptp_v2i(V)	((V) / NBPD)	/* VA => index (same as pdei) */

/*
 * PG_AVAIL usage: we make use of the ignored bits of the PTE
 */

#define PG_W		PG_AVAIL1	/* "wired" mapping */
#define PG_PVLIST	PG_AVAIL2	/* mapping has entry on pvlist */
/* PG_AVAIL3 not used */

/*
 * pmap data structures: see pmap.c for details of locking.
 */

struct pmap;
typedef struct pmap *pmap_t;

/*
 * we maintain a list of all non-kernel pmaps
 */

LIST_HEAD(pmap_head, pmap); /* struct pmap_head: head of a pmap list */

/*
 * the pmap structure
 *
 * note that the pm_obj contains the simple_lock, the reference count,
 * page list, and number of PTPs within the pmap.
 */

struct pmap {
  struct uvm_object pm_obj;	/* object (lck by object lock) */
  LIST_ENTRY(pmap) pm_list;	/* list (lck by pm_list lock) */
  pd_entry_t *pm_pdir;		/* VA of PD (lck by object lock) */
  u_int32_t pm_pdirpa;		/* PA of PD (read-only after create) */
  struct vm_page *pm_ptphint;	/* pointer to a random PTP in our pmap */
  struct pmap_statistics pm_stats;  /* pmap stats (lck by object lock) */
};

/*
 * global kernel variables
 */

/* PTDpaddr: is the physical address of the kernel's PDP */
extern u_long PTDpaddr;

extern struct pmap kernel_pmap_store;	/* kernel pmap */
extern int nkpde;			/* current # of PDEs for kernel */
extern int pmap_pg_g;			/* do we support PG_G? */

/*
 * macros
 */

#define	pmap_kernel()			(&kernel_pmap_store)
#define	pmap_resident_count(pmap)	((pmap)->pm_stats.resident_count)
#define	pmap_update()			tlbflush()

#define pmap_clear_modify(pg)		pmap_change_attrs(pg, 0, PG_M)
#define pmap_clear_reference(pg)	pmap_change_attrs(pg, 0, PG_U)
#define pmap_copy(DP,SP,D,L,S)		pmap_transfer(DP,SP,D,L,S, FALSE)
#define pmap_is_modified(pg)		pmap_test_attrs(pg, PG_M)
#define pmap_is_referenced(pg)		pmap_test_attrs(pg, PG_U)
#define pmap_move(DP,SP,D,L,S)		pmap_transfer(DP,SP,D,L,S, TRUE)
#define pmap_phys_address(ppn)		i386_ptob(ppn)
#define pmap_valid_entry(E) 		((E) & PG_V) /* is PDE or PTE valid? */

/*
 * prototypes
 */

void		pmap_activate __P((struct proc *));
void		pmap_bootstrap __P((vaddr_t));
boolean_t	pmap_change_attrs __P((struct vm_page *, int, int));
void		pmap_deactivate __P((struct proc *));
static void	pmap_kenter_pa __P((vaddr_t, paddr_t, vm_prot_t));
static void	pmap_page_protect __P((struct vm_page *, vm_prot_t));
void		pmap_page_remove  __P((struct vm_page *));
static void	pmap_protect __P((struct pmap *, vaddr_t, 
				vaddr_t, vm_prot_t));
void		pmap_remove __P((struct pmap *, vaddr_t, vaddr_t));
boolean_t	pmap_test_attrs __P((struct vm_page *, int));
void		pmap_transfer __P((struct pmap *, struct pmap *, vaddr_t, 
				   vsize_t, vaddr_t, boolean_t));
static void	pmap_update_pg __P((vaddr_t));
static void	pmap_update_2pg __P((vaddr_t,vaddr_t));
void		pmap_write_protect __P((struct pmap *, vaddr_t, 
				vaddr_t, vm_prot_t));

vaddr_t reserve_dumppages __P((vaddr_t)); /* XXX: not a pmap fn */

#define PMAP_GROWKERNEL		/* turn on pmap_growkernel interface */

/*
 * inline functions
 */

/*
 * pmap_update_pg: flush one page from the TLB (or flush the whole thing
 *	if hardware doesn't support one-page flushing)
 */

__inline static void pmap_update_pg(va)

vaddr_t va;

{
#if defined(I386_CPU)
  if (cpu_class == CPUCLASS_386)
    pmap_update();
  else
#endif
    invlpg((u_int) va);
}

/*
 * pmap_update_2pg: flush two pages from the TLB
 */

__inline static void pmap_update_2pg(va, vb)

vaddr_t va, vb;

{
#if defined(I386_CPU)
  if (cpu_class == CPUCLASS_386)
    pmap_update();
  else
#endif
    {
      invlpg((u_int) va);
      invlpg((u_int) vb);
    }
}

/*
 * pmap_page_protect: change the protection of all recorded mappings
 *	of a managed page
 *
 * => this function is a frontend for pmap_page_remove/pmap_change_attrs
 * => we only have to worry about making the page more protected.
 *	unprotecting a page is done on-demand at fault time.
 */

__inline static void pmap_page_protect(pg, prot)

struct vm_page *pg;
vm_prot_t prot;

{
  if ((prot & VM_PROT_WRITE) == 0) {
    if (prot & (VM_PROT_READ|VM_PROT_EXECUTE)) {
      (void) pmap_change_attrs(pg, PG_RO, PG_RW);  
    } else {
      pmap_page_remove(pg);
    }
  }
}

/*
 * pmap_protect: change the protection of pages in a pmap
 *
 * => this function is a frontend for pmap_remove/pmap_write_protect
 * => we only have to worry about making the page more protected.
 *	unprotecting a page is done on-demand at fault time.
 */

__inline static void pmap_protect(pmap, sva, eva, prot)

struct pmap *pmap;
vaddr_t sva, eva;
vm_prot_t prot;

{
  if ((prot & VM_PROT_WRITE) == 0) {
    if (prot & (VM_PROT_READ|VM_PROT_EXECUTE)) {
      pmap_write_protect(pmap, sva, eva, prot);
    } else {
      pmap_remove(pmap, sva, eva);
    }
  }
}

/*
 * pmap_kenter_pa: enter a kernel mapping without R/M (pv_entry) tracking
 *
 * => no need to lock anything, assume va is already allocated 
 * => should be faster than normal pmap enter function
 */
                                              
__inline static void pmap_kenter_pa(va, pa, prot)
                                               
vaddr_t va;
paddr_t pa;
vm_prot_t prot;
  
{
  struct pmap *pm = pmap_kernel();
  pt_entry_t *pte, opte;                     
  int s;

  s = splimp();
  simple_lock(&pm->pm_obj.vmobjlock);
  pm->pm_stats.resident_count++;
  pm->pm_stats.wired_count++;
  simple_unlock(&pm->pm_obj.vmobjlock);
  splx(s);

  pte = vtopte(va);     
  opte = *pte;           
  *pte = pa | ((prot & VM_PROT_WRITE)? PG_RW : PG_RO) |
	      PG_V | pmap_pg_g;         /* zap! */
  if (pmap_valid_entry(opte))
    pmap_update_pg(va);                         
}                                               

vaddr_t	pmap_map __P((vaddr_t, paddr_t, paddr_t, int));

#endif	/* PMAP_MACHINE */