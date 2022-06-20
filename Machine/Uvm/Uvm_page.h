/* Machine: uvm_page.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_PAGE_H_
#define _UVM_UVM_PAGE_H_

/*
 * uvm_page.h
 */

/*
 * macros
 */

#define uvm_lock_pageq()	simple_lock(&uvm.pageqlock)
#define uvm_unlock_pageq()	simple_unlock(&uvm.pageqlock)
#define uvm_lock_fpageq()	simple_lock(&uvm.fpageqlock)
#define uvm_unlock_fpageq()	simple_unlock(&uvm.fpageqlock)

#define uvm_pagehash(obj,off) \
	(((unsigned long)obj+(unsigned long)atop(off)) & uvm.page_hashmask)

/*
 * handle inline options
 */

#ifdef UVM_PAGE_INLINE
#define PAGE_INLINE static __inline
#else 
#define PAGE_INLINE /* nothing */
#endif /* UVM_PAGE_INLINE */

/*
 * prototypes: the following prototypes define the interface to pages
 */

void uvm_page_init __P((vaddr_t *, vaddr_t *));
#if defined(UVM_PAGE_TRKOWN)
void uvm_page_own __P((struct vm_page *, char *));
#endif
#if !defined(PMAP_STEAL_MEMORY)
boolean_t uvm_page_physget __P((paddr_t *));
#endif
void uvm_page_rehash __P((void));

PAGE_INLINE void uvm_pageactivate __P((struct vm_page *));
vaddr_t uvm_pageboot_alloc __P((vsize_t));
PAGE_INLINE void uvm_pagecopy __P((struct vm_page *, struct vm_page *));
PAGE_INLINE void uvm_pagedeactivate __P((struct vm_page *));
void uvm_pagefree __P((struct vm_page *));
PAGE_INLINE struct vm_page *uvm_pagelookup 
					__P((struct uvm_object *, vaddr_t));
void uvm_pageremove __P((struct vm_page *));
/* uvm_pagerename: not needed */
PAGE_INLINE void uvm_pageunwire __P((struct vm_page *));
PAGE_INLINE void uvm_pagewait __P((struct vm_page *, int));
PAGE_INLINE void uvm_pagewake __P((struct vm_page *));
PAGE_INLINE void uvm_pagewire __P((struct vm_page *));
PAGE_INLINE void uvm_pagezero __P((struct vm_page *));

PAGE_INLINE int uvm_page_lookup_freelist __P((struct vm_page *));

#endif /* _UVM_UVM_PAGE_H_ */