/* Machine: vm_page.h,v 1.00 2022/06/19 */

/*
 *	Resident memory system definitions.
 */
#ifndef	_VM_PAGE_
#define	_VM_PAGE_

/*
 *	Management of resident (logical) pages.
 *
 *	A small structure is kept for each resident
 *	page, indexed by page number.  Each structure
 *	is an element of several lists:
 *
 *		A hash table bucket used to quickly
 *		perform object/offset lookups
 *
 *		A list of all pages for a given object,
 *		so they can be quickly deactivated at
 *		time of deallocation.
 *
 *		An ordered list of pages due for pageout.
 *
 *	In addition, the structure contains the object
 *	and offset to which this page belongs (for pageout),
 *	and sundry status bits.
 *
 *	Fields in this structure are locked either by the lock on the
 *	object that the page belongs to (O) or by the lock on the page
 *	queues (P) [or both].
 */

/*
 * locking note: the mach version of this data structure had bit
 * fields for the flags, and the bit fields were divided into two
 * items (depending on who locked what).  some time, in BSD, the bit
 * fields were dumped and all the flags were lumped into one short.
 * that is fine for a single threaded uniprocessor OS, but bad if you
 * want to actual make use of locking (simple_lock's).  so, we've
 * seperated things back out again.
 *
 * note the page structure has no lock of its own.
 */

#include <uvm/uvm_extern.h>
#include <vm/pglist.h>

struct vm_page {
  TAILQ_ENTRY(vm_page)	pageq;		/* queue info for FIFO
					 * queue or free list (P) */
  TAILQ_ENTRY(vm_page)	hashq;		/* hash table links (O)*/
  TAILQ_ENTRY(vm_page)	listq;		/* pages in same object (O)*/

  vaddr_t		offset;		/* offset into object (O,P) */

  struct uvm_object	*uobject;	/* object (O,P) */
  struct vm_anon	*uanon;		/* anon (O,P) */
  u_short		flags;		/* object flags [O] */
  u_short		version;	/* version count [O] */
  u_short		wire_count;	/* wired down map refs [P] */
  u_short 		pqflags;	/* page queue flags [P] */
  u_int			loan_count;	/* number of active loans
					 * to read: [O or P]
					 * to modify: [O _and_ P] */
  paddr_t		phys_addr;	/* physical address of page */
#if defined(UVM_PAGE_TRKOWN)
  /* debugging fields to track page ownership */
  pid_t			owner;		/* proc that set PG_BUSY */
  char			*owner_tag;	/* why it was set busy */
#endif
};

/*
 * These are the flags defined for vm_page.
 *
 * Note: PG_FILLED and PG_DIRTY are added for the filesystems.
 */

/*
 * locking rules:
 *   PG_ ==> locked by object lock
 *   PQ_ ==> lock by page queue lock 
 *   PQ_FREE is locked by free queue lock and is mutex with all other PQs
 *
 * possible deadwood: PG_FAULTING, PQ_LAUNDRY
 */
#define	PG_CLEAN	0x0008		/* page has not been modified */
#define	PG_BUSY		0x0010		/* page is in transit  */
#define	PG_WANTED	0x0020		/* someone is waiting for page */
#define	PG_TABLED	0x0040		/* page is in VP table  */
#define	PG_FAKE		0x0200		/* page is placeholder for pagein */
#define	PG_FILLED	0x0400		/* client flag to set when filled */
#define	PG_DIRTY	0x0800		/* client flag to set when dirty */
#define PG_RELEASED	0x1000		/* page released while paging */
#define	PG_FAULTING	0x2000		/* page is being faulted in */
#define PG_CLEANCHK	0x4000		/* clean bit has been checked */

#define PQ_FREE		0x0001		/* page is on free list */
#define PQ_INACTIVE	0x0002		/* page is in inactive list */
#define PQ_ACTIVE	0x0004		/* page is in active list */
#define PQ_LAUNDRY	0x0008		/* page is being cleaned now */
#define PQ_ANON		0x0010		/* page is part of an anon, rather
					   than an uvm_object */
#define PQ_AOBJ		0x0020		/* page is part of an anonymous
					   uvm_object */
#define PQ_SWAPBACKED	(PQ_ANON|PQ_AOBJ)

/*
 * physical memory layout structure
 *
 * MD vmparam.h must #define:
 *   VM_PHYSEG_MAX = max number of physical memory segments we support
 *		   (if this is "1" then we revert to a "contig" case)
 *   VM_PHYSSEG_STRAT: memory sort/search options (for VM_PHYSEG_MAX > 1)
 * 	- VM_PSTRAT_RANDOM:   linear search (random order)
 *	- VM_PSTRAT_BSEARCH:  binary search (sorted by address)
 *	- VM_PSTRAT_BIGFIRST: linear search (sorted by largest segment first)
 *      - others?
 *   XXXCDC: eventually we should remove contig and old non-contig cases
 *   and purge all left-over global variables...
 */
#define VM_PSTRAT_RANDOM	1
#define VM_PSTRAT_BSEARCH	2
#define VM_PSTRAT_BIGFIRST	3

/*
 * vm_physmemseg: describes one segment of physical memory
 */
struct vm_physseg {
	vaddr_t		start;		/* PF# of first page in segment */
	vaddr_t		end;		/* (PF# of last page in segment) + 1 */
	vaddr_t		avail_start;	/* PF# of first free page in segment */
	vaddr_t		avail_end;	/* (PF# of last free page in segment) +1  */
	int	free_list;		/* which free list they belong on */
	struct	vm_page *pgs;		/* vm_page structures (from start) */
	struct	vm_page *lastpg;	/* vm_page structure for end */
	struct	pmap_physseg pmseg;	/* pmap specific (MD) data */
};

#if defined(_KERNEL)

/*
 *	Each pageable resident page falls into one of three lists:
 *
 *	free	
 *		Available for allocation now.
 *	inactive
 *		Not referenced in any map, but still has an
 *		object/offset-page mapping, and may be dirty.
 *		This is the list of pages that should be
 *		paged out next.
 *	active
 *		A list of pages which have been placed in
 *		at least one physical map.  This list is
 *		ordered, in LRU-like fashion.
 */

extern
struct pglist	vm_page_queue_free;	/* memory free queue */
extern
struct pglist	vm_page_queue_active;	/* active memory queue */
extern
struct pglist	vm_page_queue_inactive;	/* inactive memory queue */


/*
 * physical memory config is stored in vm_physmem.
 */

extern struct vm_physseg vm_physmem[VM_PHYSSEG_MAX];
extern int vm_nphysseg;

/*
 * prototypes
 */

static struct vm_page *PHYS_TO_VM_PAGE __P((paddr_t));
static int vm_physseg_find __P((paddr_t, int *));

void		 vm_page_activate __P((vm_page_t));
vm_page_t	 vm_page_alloc __P((vm_object_t, vaddr_t));
vm_page_t	 vm_page_alloc1 __P((void));
int		 vm_page_alloc_memory __P((psize_t size, paddr_t low,
			paddr_t high, paddr_t alignment, paddr_t boundary,
			struct pglist *rlist, int nsegs, int waitok));
void		 vm_page_free_memory __P((struct pglist *list));
void		 vm_page_bootstrap __P((vaddr_t *, vaddr_t *));
void		 vm_page_copy __P((vm_page_t, vm_page_t));
void		 vm_page_deactivate __P((vm_page_t));
void		 vm_page_free __P((vm_page_t));
void		 vm_page_free1 __P((vm_page_t));
void		 vm_page_insert __P((vm_page_t, vm_object_t, vaddr_t));
vm_page_t	 vm_page_lookup __P((vm_object_t, vaddr_t));
void		 vm_page_physload __P((paddr_t, paddr_t,
					paddr_t, paddr_t));
void		 vm_page_physrehash __P((void));
void		 vm_page_remove __P((vm_page_t));
void		 vm_page_rename __P((vm_page_t, vm_object_t, vaddr_t));
void		 vm_page_unwire __P((vm_page_t));
void		 vm_page_wire __P((vm_page_t));
boolean_t	 vm_page_zero_fill __P((vm_page_t));

/*
 * macros and inlines
 */
#define VM_PAGE_TO_PHYS(entry)	((entry)->phys_addr)

/*
 * when VM_PHYSSEG_MAX is 1, we can simplify these functions
 */

/*
 * vm_physseg_find: find vm_physseg structure that belongs to a PA
 */
static __inline int
vm_physseg_find(pframe, offp)
	paddr_t pframe;
	int	*offp;
{
#if VM_PHYSSEG_MAX == 1

	/* 'contig' case */
	if (pframe >= vm_physmem[0].start && pframe < vm_physmem[0].end) {
		if (offp)
			*offp = pframe - vm_physmem[0].start;
		return(0);
	}
	return(-1);

#elif (VM_PHYSSEG_STRAT == VM_PSTRAT_BSEARCH)
	/* binary search for it */
	int	start, len, try;

	/*
	 * if try is too large (thus target is less than than try) we reduce
	 * the length to trunc(len/2) [i.e. everything smaller than "try"]
	 *
	 * if the try is too small (thus target is greater than try) then
	 * we set the new start to be (try + 1).   this means we need to
	 * reduce the length to (round(len/2) - 1).
	 *
	 * note "adjust" below which takes advantage of the fact that
	 *  (round(len/2) - 1) == trunc((len - 1) / 2)
	 * for any value of len we may have
	 */

	for (start = 0, len = vm_nphysseg ; len != 0 ; len = len / 2) {
		try = start + (len / 2);	/* try in the middle */

		/* start past our try? */
		if (pframe >= vm_physmem[try].start) {
			/* was try correct? */
			if (pframe < vm_physmem[try].end) {
				if (offp)
					*offp = pframe - vm_physmem[try].start;
				return(try);            /* got it */
			}
			start = try + 1;	/* next time, start here */
			len--;			/* "adjust" */
		} else {
			/*
			 * pframe before try, just reduce length of
			 * region, done in "for" loop
			 */
		}
	}
	return(-1);

#else
	/* linear search for it */
	int	lcv;

	for (lcv = 0; lcv < vm_nphysseg; lcv++) {
		if (pframe >= vm_physmem[lcv].start &&
		    pframe < vm_physmem[lcv].end) {
			if (offp)
				*offp = pframe - vm_physmem[lcv].start;
			return(lcv);		   /* got it */
		}
	}
	return(-1);

#endif
}


/*
 * IS_VM_PHYSADDR: only used my mips/pmax/pica trap/pmap.
 */

#define IS_VM_PHYSADDR(PA) (vm_physseg_find(atop(PA), NULL) != -1)

/*
 * PHYS_TO_VM_PAGE: find vm_page for a PA.   used by MI code to get vm_pages
 * back from an I/O mapping (ugh!).   used in some MD code as well.
 */
static __inline struct vm_page *
PHYS_TO_VM_PAGE(pa)
	paddr_t pa;
{
	paddr_t pf = atop(pa);
	int	off;
	int	psi;

	psi = vm_physseg_find(pf, &off);
	if (psi != -1)
		return(&vm_physmem[psi].pgs[off]);
	return(NULL);
}

#define VM_PAGE_IS_FREE(entry)  ((entry)->pqflags & PQ_FREE)

extern
simple_lock_data_t	vm_page_queue_lock;	/* lock on active and inactive
						   page queues */
extern						/* lock on free page queue */
simple_lock_data_t	vm_page_queue_free_lock;

#define PAGE_ASSERT_WAIT(m, interruptible)	{ \
				(m)->flags |= PG_WANTED; \
				assert_wait((m), (interruptible)); \
			}

#define PAGE_WAKEUP(m)	{ \
				(m)->flags &= ~PG_BUSY; \
				if ((m)->flags & PG_WANTED) { \
					(m)->flags &= ~PG_WANTED; \
					thread_wakeup((m)); \
				} \
			}

#define	vm_page_lock_queues()	simple_lock(&vm_page_queue_lock)
#define	vm_page_unlock_queues()	simple_unlock(&vm_page_queue_lock)

#define vm_page_set_modified(m)	{ (m)->flags &= ~PG_CLEAN; }

#define	VM_PAGE_INIT(mem, obj, offset) { \
	(mem)->flags = PG_BUSY | PG_CLEAN | PG_FAKE; \
	if (obj) \
		vm_page_insert((mem), (obj), (offset)); \
	else \
		(mem)->object = NULL; \
	(mem)->wire_count = 0; \
}

#if VM_PAGE_DEBUG

/*
 * VM_PAGE_CHECK: debugging check of a vm_page structure
 */
static __inline void
VM_PAGE_CHECK(mem)
	struct vm_page *mem;
{
	int lcv;

	for (lcv = 0 ; lcv < vm_nphysseg ; lcv++) {
		if ((unsigned int) mem >= (unsigned int) vm_physmem[lcv].pgs &&
		    (unsigned int) mem <= (unsigned int) vm_physmem[lcv].lastpg)
			break;
	}
	if (lcv == vm_nphysseg ||
	    (mem->flags & (PG_ACTIVE|PG_INACTIVE)) == (PG_ACTIVE|PG_INACTIVE))
		panic("vm_page_check: not valid!"); 
	return;
}

#else /* VM_PAGE_DEBUG */
#define	VM_PAGE_CHECK(mem)
#endif /* VM_PAGE_DEBUG */

#endif /* _KERNEL */
#endif /* !_VM_PAGE_ */