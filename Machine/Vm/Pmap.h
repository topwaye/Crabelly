/* Machine: pmap.h,v 1.00 2022/06/19 */

/*
 *	Machine address mapping definitions -- machine-independent
 *	section.  [For machine-dependent section, see "machine/pmap.h".]
 */

#ifndef	_PMAP_VM_
#define	_PMAP_VM_

#if defined(_KERNEL) && !defined(_LKM)
#include "opt_pmap_new.h"
#endif

struct proc;		/* for pmap_activate()/pmap_deactivate() proto */

/*
 * Each machine dependent implementation is expected to
 * keep certain statistics.  They may do this anyway they
 * so choose, but are expected to return the statistics
 * in the following structure.
 */
struct pmap_statistics {
	long		resident_count;	/* # of pages mapped (total)*/
	long		wired_count;	/* # of pages wired */
};
typedef struct pmap_statistics	*pmap_statistics_t;

#include <machine/pmap.h>

/*
 * PMAP_PGARG hack
 *
 * operations that take place on managed pages used to take PAs.
 * this caused us to translate the PA back to a page (or pv_head).
 * PMAP_NEW avoids this by passing the vm_page in (pv_head should be
 * pointed to by vm_page (or be a part of it)).
 *
 * applies to: pmap_page_protect, pmap_is_referenced, pmap_is_modified,
 * pmap_clear_reference, pmap_clear_modify.
 *
 * the latter two functions are boolean_t in PMAP_NEW.  they return
 * TRUE if something was cleared.
 */
#if defined(PMAP_NEW)
#define PMAP_PGARG(PG) (PG)
#else
#define PMAP_PGARG(PG) (VM_PAGE_TO_PHYS(PG))
#endif

#ifndef PMAP_EXCLUDE_DECLS	/* Used in Sparc port to virtualize pmap mod */
#ifdef _KERNEL
__BEGIN_DECLS
void		*pmap_bootstrap_alloc __P((int));
void		 pmap_activate __P((struct proc *));
void		 pmap_deactivate __P((struct proc *));
void		 pmap_change_wiring __P((pmap_t, vaddr_t, boolean_t));

#if defined(PMAP_NEW)
#if !defined(pmap_clear_modify)
boolean_t	 pmap_clear_modify __P((struct vm_page *));
#endif
#if !defined(pmap_clear_reference)
boolean_t	 pmap_clear_reference __P((struct vm_page *));
#endif
#else	/* PMAP_NEW */
void		 pmap_clear_modify __P((paddr_t pa));
void		 pmap_clear_reference __P((paddr_t pa));
#endif	/* PMAP_NEW */

void		 pmap_collect __P((pmap_t));
void		 pmap_copy __P((pmap_t,
		    pmap_t, vaddr_t, vsize_t, vaddr_t));
void		 pmap_copy_page __P((paddr_t, paddr_t));
#if defined(PMAP_NEW)
struct pmap 	 *pmap_create __P((void));
#else
pmap_t		 pmap_create __P((vsize_t));
#endif
void		 pmap_destroy __P((pmap_t));
void		 pmap_enter __P((pmap_t,
		    vaddr_t, paddr_t, vm_prot_t, boolean_t, vm_prot_t));
paddr_t		pmap_extract __P((pmap_t, vaddr_t));
#if defined(PMAP_NEW) && defined(PMAP_GROWKERNEL)
void		 pmap_growkernel __P((vaddr_t));
#endif

void		 pmap_init __P((void));

#if defined(PMAP_NEW)
void		 pmap_kenter_pa __P((vaddr_t, paddr_t, vm_prot_t));
void		 pmap_kenter_pgs __P((vaddr_t, struct vm_page **, int));
void		 pmap_kremove __P((vaddr_t, vsize_t));
#if !defined(pmap_is_modified)
boolean_t	 pmap_is_modified __P((struct vm_page *));
#endif
#if !defined(pmap_is_referenced)
boolean_t	 pmap_is_referenced __P((struct vm_page *));
#endif
#else	/* PMAP_NEW */
boolean_t	 pmap_is_modified __P((paddr_t pa));
boolean_t	 pmap_is_referenced __P((paddr_t pa));
#endif	/* PMAP_NEW */

#if defined(PMAP_NEW)
void		 pmap_page_protect __P((struct vm_page *, vm_prot_t));
#else
void		 pmap_page_protect __P((paddr_t, vm_prot_t));
#endif

void		 pmap_pageable __P((pmap_t,
		    vaddr_t, vaddr_t, boolean_t));
#if !defined(pmap_phys_address)
paddr_t	 pmap_phys_address __P((int));
#endif
void		 pmap_protect __P((pmap_t,
		    vaddr_t, vaddr_t, vm_prot_t));
void		 pmap_reference __P((pmap_t));
void		 pmap_remove __P((pmap_t, vaddr_t, vaddr_t));
void		 pmap_update __P((void));
void		 pmap_zero_page __P((paddr_t));

#if defined(PMAP_STEAL_MEMORY)
vaddr_t	 pmap_steal_memory __P((vsize_t, paddr_t *,
		    paddr_t *));
#else
void		 pmap_virtual_space __P((vaddr_t *, vaddr_t *));
#endif
__END_DECLS
#endif	/* kernel*/
#endif  /* PMAP_EXCLUDE_DECLS */

#endif /* _PMAP_VM_ */