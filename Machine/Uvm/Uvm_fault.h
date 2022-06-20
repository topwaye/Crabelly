/* Machine: uvm_fault.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_FAULT_H_
#define _UVM_UVM_FAULT_H_

/*
 * fault types
 */

#define VM_FAULT_INVALID ((vm_fault_t) 0x0)	/* invalid mapping */
#define VM_FAULT_PROTECT ((vm_fault_t) 0x1)	/* protection */
#define VM_FAULT_WIRE	 ((vm_fault_t) 0x2)	/* wire mapping */

/*
 * fault data structures
 */

/*
 * uvm_faultinfo: to load one of these fill in all orig_* fields and
 * then call uvmfault_lookup on it.
 */


struct uvm_faultinfo {
	vm_map_t orig_map;		/* IN: original map */
	vaddr_t orig_rvaddr;		/* IN: original rounded VA */
	vsize_t orig_size;		/* IN: original size of interest */
	vm_map_t map;			/* map (could be a submap) */
	unsigned int mapv;		/* map's version number */
	vm_map_entry_t entry;		/* map entry (from 'map') */
	vsize_t size;			/* size of interest */
};

/*
 * fault prototypes
 */


int uvmfault_anonget __P((struct uvm_faultinfo *, struct vm_amap *,
													struct vm_anon *));
static boolean_t uvmfault_lookup __P((struct uvm_faultinfo *, boolean_t));
static boolean_t uvmfault_relock __P((struct uvm_faultinfo *));
static void uvmfault_unlockall __P((struct uvm_faultinfo *, struct vm_amap *,
			            struct uvm_object *, struct vm_anon *));
static void uvmfault_unlockmaps __P((struct uvm_faultinfo *, boolean_t));

int uvm_fault_wire __P((vm_map_t, vaddr_t, vaddr_t));
void uvm_fault_unwire __P((struct pmap *, vaddr_t, vaddr_t));

#endif /* _UVM_UVM_FAULT_H_ */