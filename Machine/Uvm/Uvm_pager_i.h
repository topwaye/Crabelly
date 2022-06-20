/* Machine: uvm_pager_i.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_PAGER_I_H_
#define _UVM_UVM_PAGER_I_H_

/*
 * uvm_pager_i.h
 */

/*
 * inline functions [maybe]
 */

#if defined(UVM_PAGER_INLINE) || defined(UVM_PAGER)

/*
 * uvm_pageratop: convert KVAs in the pager map back to their page
 * structures.
 */

PAGER_INLINE struct vm_page *
uvm_pageratop(kva)
	vaddr_t kva;
{
	paddr_t pa;
 
	pa = pmap_extract(pmap_kernel(), kva);
	if (pa == 0)
		panic("uvm_pageratop");
	return (PHYS_TO_VM_PAGE(pa));
} 

#endif /* defined(UVM_PAGER_INLINE) || defined(UVM_PAGER) */

#endif /* _UVM_UVM_PAGER_I_H_ */