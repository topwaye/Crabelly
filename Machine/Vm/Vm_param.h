/* Machine: vm_param.h,v 1.00 2022/06/19 */

/*
 *	Machine independent virtual memory parameters.
 */

#ifndef	_VM_PARAM_
#define	_VM_PARAM_

#include <machine/vmparam.h>

/*
 * This belongs in types.h, but breaks too many existing programs.
 */
typedef int	boolean_t;
#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

/*
 *	The machine independent pages are refered to as PAGES.  A page
 *	is some number of hardware pages, depending on the target machine.
 */
#define	DEFAULT_PAGE_SIZE	4096

#if defined(_KERNEL)
/*
 *	All references to the size of a page should be done with PAGE_SIZE
 *	or PAGE_SHIFT.  The fact they are variables is hidden here so that
 *	we can easily make them constant if we so desire.
 */
#define	PAGE_SIZE	uvmexp.pagesize		/* size of page */
#define	PAGE_MASK	uvmexp.pagemask		/* size of page - 1 */
#define	PAGE_SHIFT	uvmexp.pageshift	/* bits to shift for pages */
#endif /* _KERNEL */

/*
 * CTL_VM identifiers
 */
#define	VM_METER	1		/* struct vmmeter */
#define	VM_LOADAVG	2		/* struct loadavg */
#define VM_UVMEXP	3		/* struct uvmexp */
#define	VM_MAXID	4		/* number of valid vm ids */

#define	CTL_VM_NAMES { \
	{ 0, 0 }, \
	{ "vmmeter", CTLTYPE_STRUCT }, \
	{ "loadavg", CTLTYPE_STRUCT }, \
	{ "uvmexp", CTLTYPE_STRUCT }, \
}


/* 
 *	Return values from the VM routines.
 */
#define	KERN_SUCCESS		0
#define	KERN_INVALID_ADDRESS	1
#define	KERN_PROTECTION_FAILURE	2
#define	KERN_NO_SPACE		3
#define	KERN_INVALID_ARGUMENT	4
#define	KERN_FAILURE		5
#define	KERN_RESOURCE_SHORTAGE	6
#define	KERN_NOT_RECEIVER	7
#define	KERN_NO_ACCESS		8
#define	KERN_PAGES_LOCKED	9

#ifndef ASSEMBLER
/*
 *	Convert addresses to pages and vice versa.
 *	No rounding is used.
 */
#ifdef _KERNEL
#define	atop(x)		(((unsigned long)(x)) >> PAGE_SHIFT)
#define	ptoa(x)		((vaddr_t)((x) << PAGE_SHIFT))

/*
 * Round off or truncate to the nearest page.  These will work
 * for either addresses or counts (i.e., 1 byte rounds to 1 page).
 */
#define	round_page(x) \
	((vaddr_t)((((vaddr_t)(x)) + PAGE_MASK) & ~PAGE_MASK))
#define	trunc_page(x) \
	((vaddr_t)(((vaddr_t)(x)) & ~PAGE_MASK))
#define	num_pages(x) \
	((vaddr_t)((((vaddr_t)(x)) + PAGE_MASK) >> PAGE_SHIFT))

extern psize_t		mem_size;	/* size of physical memory (bytes) */
extern paddr_t		first_addr;	/* first physical page */
extern paddr_t		last_addr;	/* last physical page */

#else
/* out-of-kernel versions of round_page and trunc_page */
#define	round_page(x) \
	((((vaddr_t)(x) + (vm_page_size - 1)) / vm_page_size) * \
	    vm_page_size)
#define	trunc_page(x) \
	((((vaddr_t)(x)) / vm_page_size) * vm_page_size)

#endif /* _KERNEL */
#endif /* ASSEMBLER */
#endif /* _VM_PARAM_ */