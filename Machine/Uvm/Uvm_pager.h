/* Machine: uvm_pager.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_PAGER_H_
#define _UVM_UVM_PAGER_H_

/*
 * uvm_pager.h
 */

/*
 * async pager i/o descriptor structure
 */

TAILQ_HEAD(uvm_aiohead, uvm_aiodesc);

struct uvm_aiodesc {
	void (*aiodone) __P((struct uvm_aiodesc *));
						/* aio done function */
	vaddr_t kva;			/* KVA of mapped page(s) */
	int npages;				/* # of pages in I/O req */
	void *pd_ptr;				/* pager-dependent pointer */
	TAILQ_ENTRY(uvm_aiodesc) aioq;		/* linked list of aio's */
};

/*
 * pager ops
 */

struct uvm_pagerops {
	void		(*pgo_init) __P((void));/* init pager */
	void		(*pgo_reference)	/* add reference to obj */
			 __P((struct uvm_object *));		
	void			(*pgo_detach)	/* drop reference to obj */
			 __P((struct uvm_object *));
	int			(*pgo_fault)	/* special nonstd fault fn */
			 __P((struct uvm_faultinfo *, vaddr_t,
				 vm_page_t *, int, int, vm_fault_t,
				 vm_prot_t, int));
	boolean_t		(*pgo_flush)	/* flush pages out of obj */
			 __P((struct uvm_object *, vaddr_t, 
				vaddr_t, int));
	int			(*pgo_get)	/* get/read page */
			 __P((struct uvm_object *, vaddr_t, 
				 vm_page_t *, int *, int, vm_prot_t, int, int));
	int			(*pgo_asyncget)	/* start async get */
			 __P((struct uvm_object *, vaddr_t, int));
	int			(*pgo_put)	/* put/write page */
			 __P((struct uvm_object *, vm_page_t *, 
				 int, boolean_t));
	void			(*pgo_cluster)	/* return range of cluster */
			__P((struct uvm_object *, vaddr_t, vaddr_t *,
				vaddr_t *));
	struct vm_page **	(*pgo_mk_pcluster)	/* make "put" cluster */
			 __P((struct uvm_object *, struct vm_page **,
				 int *, struct vm_page *, int, vaddr_t,
				 vaddr_t));
	void			(*pgo_shareprot)	/* share protect */
			 __P((vm_map_entry_t, vm_prot_t));
	void			(*pgo_aiodone)		/* async iodone */
			 __P((struct uvm_aiodesc *));
	boolean_t		(*pgo_releasepg)	/* release page */
			 __P((struct vm_page *, struct vm_page **));
};

/* pager flags [mostly for flush] */

#define PGO_CLEANIT	0x001	/* write dirty pages to backing store */
#define PGO_SYNCIO	0x002	/* if PGO_CLEAN: use sync I/O? */
/*
 * obviously if neither PGO_INVALIDATE or PGO_FREE are set then the pages
 * stay where they are.
 */
#define PGO_DEACTIVATE	0x004	/* deactivate flushed pages */
#define PGO_FREE	0x008	/* free flushed pages */

#define PGO_ALLPAGES	0x010	/* flush whole object/get all pages */
#define PGO_DOACTCLUST	0x020	/* flag to mk_pcluster to include active */
#define PGO_LOCKED	0x040	/* fault data structures are locked [get] */
#define PGO_PDFREECLUST	0x080	/* daemon's free cluster flag [uvm_pager_put] */
#define PGO_REALLOCSWAP	0x100	/* reallocate swap area [pager_dropcluster] */

/* page we are not interested in getting */
#define PGO_DONTCARE ((struct vm_page *) -1)	/* [get only] */

/*
 * handle inline options
 */

#ifdef UVM_PAGER_INLINE
#define PAGER_INLINE static __inline
#else 
#define PAGER_INLINE /* nothing */
#endif /* UVM_PAGER_INLINE */

/*
 * prototypes
 */

void		uvm_pager_dropcluster __P((struct uvm_object *, 
					struct vm_page *, struct vm_page **, 
					int *, int, int));
void		uvm_pager_init __P((void));
int		uvm_pager_put __P((struct uvm_object *, struct vm_page *, 
				   struct vm_page ***, int *, int, 
				   vaddr_t, vaddr_t));

PAGER_INLINE struct vm_page *uvm_pageratop __P((vaddr_t));

vaddr_t	uvm_pagermapin __P((struct vm_page **, int, 
				    struct uvm_aiodesc **, int));
void		uvm_pagermapout __P((vaddr_t, int));
struct vm_page **uvm_mk_pcluster  __P((struct uvm_object *, struct vm_page **,
				       int *, struct vm_page *, int, 
				       vaddr_t, vaddr_t));
void		uvm_shareprot __P((vm_map_entry_t, vm_prot_t));


#endif /* _UVM_UVM_PAGER_H_ */