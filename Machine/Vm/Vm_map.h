/* Machine: vm_map.h,v 1.00 2022/06/19 */

/*
 *	Virtual memory map module definitions.
 */

#ifndef	_VM_MAP_
#define	_VM_MAP_

#include <uvm/uvm_anon.h>

/*
 *	Types defined:
 *
 *	vm_map_t		the high-level address map data structure.
 *	vm_map_entry_t		an entry in an address map.
 *	vm_map_version_t	a timestamp of a map, for use with vm_map_lookup
 */

/*
 *	Objects which live in maps may be either VM objects, or
 *	another map (called a "sharing map") which denotes read-write
 *	sharing with other maps.
 *
 * XXXCDC: private pager data goes here now
 */

union vm_map_object {
	struct uvm_object	*uvm_obj;	/* UVM OBJECT */
	struct vm_map		*sub_map;	/* belongs to another map */
};

/*
 *	Address map entries consist of start and end addresses,
 *	a VM object (or sharing map) and offset into that object,
 *	and user-exported inheritance and protection information.
 *	Also included is control information for virtual copy operations.
 */
struct vm_map_entry {
	struct vm_map_entry	*prev;		/* previous entry */
	struct vm_map_entry	*next;		/* next entry */
	vaddr_t			start;		/* start address */
	vaddr_t			end;		/* end address */
	union vm_map_object	object;		/* object I point to */
	vsize_t			offset;		/* offset into object */
	/* etype is a bitmap that replaces the following 4 items */
	int			etype;		/* entry type */
		/* Only in task maps: */
	vm_prot_t		protection;	/* protection code */
	vm_prot_t		max_protection;	/* maximum protection */
	vm_inherit_t		inheritance;	/* inheritance */
	int			wired_count;	/* can be paged if = 0 */
	struct vm_aref		aref;		/* anonymous overlay */
	int			advice;		/* madvise advice */
#define uvm_map_entry_stop_copy flags
	u_int8_t		flags;		/* flags */

#define UVM_MAP_STATIC		0x01		/* static map entry */

};

/*
 *	Maps are doubly-linked lists of map entries, kept sorted
 *	by address.  A single hint is provided to start
 *	searches again from the last successful search,
 *	insertion, or removal.
 */
struct vm_map {
	struct pmap *		pmap;		/* Physical map */
	lock_data_t		lock;		/* Lock for map data */
	struct vm_map_entry	header;		/* List of entries */
	int			nentries;	/* Number of entries */
	vsize_t			size;		/* virtual size */
	int			ref_count;	/* Reference count */
	simple_lock_data_t	ref_lock;	/* Lock for ref_count field */
	vm_map_entry_t		hint;		/* hint for quick lookups */
	simple_lock_data_t	hint_lock;	/* lock for hint storage */
	vm_map_entry_t		first_free;	/* First free space hint */
	boolean_t		entries_pageable; /* map entries pageable?? */
	unsigned int		timestamp;	/* Version number */
#define	min_offset		header.start
#define max_offset		header.end
};

/*
 *	Macros:		vm_map_lock, etc.
 *	Function:
 *		Perform locking on the data portion of a map.
 */

#include <sys/time.h>
#include <sys/proc.h>	/* XXX for curproc and p_pid */

#define	vm_map_lock_drain_interlock(map) { \
	lockmgr(&(map)->lock, LK_DRAIN|LK_INTERLOCK, \
		&(map)->ref_lock); \
	(map)->timestamp++; \
}
#ifdef DIAGNOSTIC
#define	vm_map_lock(map) { \
	if (lockmgr(&(map)->lock, LK_EXCLUSIVE, (void *)0) != 0) { \
		panic("vm_map_lock: failed to get lock"); \
	} \
	(map)->timestamp++; \
}
#else
#define	vm_map_lock(map) { \
	lockmgr(&(map)->lock, LK_EXCLUSIVE, (void *)0); \
	(map)->timestamp++; \
}
#endif /* DIAGNOSTIC */
#define	vm_map_unlock(map) \
		lockmgr(&(map)->lock, LK_RELEASE, (void *)0)
#define	vm_map_lock_read(map) \
		lockmgr(&(map)->lock, LK_SHARED, (void *)0)
#define	vm_map_unlock_read(map) \
		lockmgr(&(map)->lock, LK_RELEASE, (void *)0)
#define vm_map_set_recursive(map) { \
	simple_lock(&(map)->lk_interlock); \
	(map)->lk_flags |= LK_CANRECURSE; \
	simple_unlock(&(map)->lk_interlock); \
}
#define vm_map_clear_recursive(map) { \
	simple_lock(&(map)->lk_interlock); \
	if ((map)->lk_exclusivecount <= 1) \
		(map)->lk_flags &= ~LK_CANRECURSE; \
	simple_unlock(&(map)->lk_interlock); \
}

#ifdef _KERNEL
/* XXX: clean up later */
static __inline boolean_t vm_map_lock_try __P((vm_map_t));

static __inline boolean_t
vm_map_lock_try(map)
	vm_map_t map;
{
	if (lockmgr(&(map)->lock, LK_EXCLUSIVE|LK_NOWAIT, (void *)0) != 0)
		return(FALSE);
	map->timestamp++;
	return(TRUE);
}
#endif

/*
 *	Functions implemented as macros
 */
#define		vm_map_min(map)		((map)->min_offset)
#define		vm_map_max(map)		((map)->max_offset)
#define		vm_map_pmap(map)	((map)->pmap)

/* XXX: number of kernel maps and entries to statically allocate */
#define MAX_KMAP	10

#if !defined(MAX_KMAPENT)
#if (50 + (2 * NPROC) > 1000)
#define MAX_KMAPENT (50 + (2 * NPROC))
#else
#define	MAX_KMAPENT	1000  /* XXXCDC: no crash */
#endif
#endif	/* !defined MAX_KMAPENT */


#ifdef _KERNEL
boolean_t	 vm_map_check_protection __P((vm_map_t,
		    vaddr_t, vaddr_t, vm_prot_t));
int		 vm_map_copy __P((vm_map_t, vm_map_t, vaddr_t,
		    vsize_t, vaddr_t, boolean_t, boolean_t));
void		 vm_map_copy_entry __P((vm_map_t,
		    vm_map_t, vm_map_entry_t, vm_map_entry_t));
struct pmap;
vm_map_t	 vm_map_create __P((struct pmap *,
		    vaddr_t, vaddr_t, boolean_t));
void		 vm_map_deallocate __P((vm_map_t));
int		 vm_map_delete __P((vm_map_t, vaddr_t, vaddr_t));
vm_map_entry_t	 vm_map_entry_create __P((vm_map_t));
void		 vm_map_entry_delete __P((vm_map_t, vm_map_entry_t));
void		 vm_map_entry_dispose __P((vm_map_t, vm_map_entry_t));
void		 vm_map_entry_unwire __P((vm_map_t, vm_map_entry_t));
int		 vm_map_find __P((vm_map_t, vm_object_t,
		    vaddr_t, vaddr_t *, vsize_t, boolean_t));
int		 vm_map_findspace __P((vm_map_t,
		    vaddr_t, vsize_t, vaddr_t *));
int		 vm_map_inherit __P((vm_map_t,
		    vaddr_t, vaddr_t, vm_inherit_t));
void		 vm_map_init __P((struct vm_map *,
		    vaddr_t, vaddr_t, boolean_t));
int		 vm_map_insert __P((vm_map_t,
		    vm_object_t, vaddr_t, vaddr_t, vaddr_t));
int		 vm_map_lookup __P((vm_map_t *, vaddr_t, vm_prot_t,
		    vm_map_entry_t *, vm_object_t *, vaddr_t *, vm_prot_t *,
		    boolean_t *, boolean_t *));
void		 vm_map_lookup_done __P((vm_map_t, vm_map_entry_t));
boolean_t	 vm_map_lookup_entry __P((vm_map_t,
		    vaddr_t, vm_map_entry_t *));
int		 vm_map_pageable __P((vm_map_t,
		    vaddr_t, vaddr_t, boolean_t));
int		 vm_map_clean __P((vm_map_t,
		    vaddr_t, vaddr_t, boolean_t, boolean_t));
void		 vm_map_print __P((vm_map_t, boolean_t));
void		 _vm_map_print __P((vm_map_t, boolean_t,
		    void (*)(const char *, ...)));
int		 vm_map_protect __P((vm_map_t,
		    vaddr_t, vaddr_t, vm_prot_t, boolean_t));
void		 vm_map_reference __P((vm_map_t));
int		 vm_map_remove __P((vm_map_t, vaddr_t, vaddr_t));
void		 vm_map_simplify __P((vm_map_t, vaddr_t));
void		 vm_map_simplify_entry __P((vm_map_t, vm_map_entry_t));
void		 vm_map_startup __P((void));
int		 vm_map_submap __P((vm_map_t,
		    vaddr_t, vaddr_t, vm_map_t));
#endif
#endif /* _VM_MAP_ */