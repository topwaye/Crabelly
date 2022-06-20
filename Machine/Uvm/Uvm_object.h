/* Machine: uvm_object.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_OBJECT_H_
#define _UVM_UVM_OBJECT_H_

/*
 * uvm_object.h
 */

/*
 * uvm_object: all that is left of mach objects.
 */

struct uvm_object {
	simple_lock_data_t	vmobjlock;	/* lock on memq */
	struct uvm_pagerops	*pgops;		/* pager ops */
	struct pglist		memq;		/* pages in this object */
	int			uo_npages;	/* # of pages in memq */
	int			uo_refs;	/* reference count */
};

/*
 * UVM_OBJ_KERN is a 'special' uo_refs value which indicates that the
 * object is a kernel memory object rather than a normal one (kernel
 * memory objects don't have reference counts -- they never die).
 *
 * this value is used to detected kernel object mappings at uvm_unmap()
 * time.   normally when an object is unmapped its pages eventaully become
 * deactivated and then paged out and/or freed.    this is not useful
 * for kernel objects... when a kernel object is unmapped we always want
 * to free the resources associated with the mapping.   UVM_OBJ_KERN
 * allows us to decide which type of unmapping we want to do.
 */
#define UVM_OBJ_KERN	(-2)

#endif /* _UVM_UVM_OBJECT_H_ */