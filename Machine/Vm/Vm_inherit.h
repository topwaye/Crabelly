/* Machine: vm_inherit.h,v 1.00 2022/06/19 */

/*
 *	Virtual memory map inheritance definitions.
 */

#ifndef	_VM_INHERIT_
#define	_VM_INHERIT_

/*
 *	Enumeration of valid values for vm_inherit_t.
 */

#define	VM_INHERIT_SHARE	((vm_inherit_t) 0)	/* share with child */
#define	VM_INHERIT_COPY		((vm_inherit_t) 1)	/* copy into child */
#define VM_INHERIT_NONE		((vm_inherit_t) 2)	/* absent from child */
#define	VM_INHERIT_DONATE_COPY	((vm_inherit_t) 3)	/* copy and delete */

#define VM_INHERIT_DEFAULT	VM_INHERIT_COPY

#endif /* _VM_INHERIT_ */