/* Machine: vm_object.h,v 1.00 2022/06/19 */

/*
 *	Virtual memory object module definitions.
 */

#ifndef	_VM_OBJECT_
#define	_VM_OBJECT_

#include <vm/vm_page.h>
#include <vm/vm_pager.h>

/*
 *	Types defined:
 *
 *	vm_object_t		Virtual memory object.
 */

struct vm_object {
	struct pglist		memq;		/* Resident memory */
	TAILQ_ENTRY(vm_object)	object_list;	/* list of all objects */
	u_short			flags;		/* see below */
	u_short			paging_in_progress; /* Paging (in or out) so
						    don't collapse or destroy */
	simple_lock_data_t	Lock;		/* Synchronization */
	int			ref_count;	/* How many refs?? */
	vsize_t		size;		/* Object size */
	int			resident_page_count;
						/* number of resident pages */
	struct vm_object	*copy;		/* Object that holds copies of
						   my changed pages */
	vm_pager_t		pager;		/* Where to get data */
	vaddr_t		paging_offset;	/* Offset into paging space */
	struct vm_object	*shadow;	/* My shadow */
	vaddr_t		shadow_offset;	/* Offset in shadow */
	TAILQ_ENTRY(vm_object)	cached_list;	/* for persistence */
	LIST_HEAD(, vm_object)	shadowers;	/* set of shadowers */
	LIST_ENTRY(vm_object)	shadowers_list;	/* link to next shadower of
						   this object's shadow */
};

/*
 * Flags
 */
#define OBJ_CANPERSIST	0x0001	/* allow to persist */
#define OBJ_INTERNAL	0x0002	/* internally created object */
#define OBJ_ACTIVE	0x0004	/* used to mark active objects */
#define OBJ_FADING	0x0008	/* tell others that the object is going away */
#define	OBJ_WAITING	0x8000	/* someone is waiting for paging to finish */

TAILQ_HEAD(vm_object_hash_head, vm_object_hash_entry);

struct vm_object_hash_entry {
	TAILQ_ENTRY(vm_object_hash_entry)  hash_links;	/* hash chain links */
	vm_object_t			   object;	/* object represented */
};

typedef struct vm_object_hash_entry	*vm_object_hash_entry_t;

#ifdef	_KERNEL
TAILQ_HEAD(object_q, vm_object);

struct object_q	vm_object_cached_list;	/* list of objects persisting */
int		vm_object_cached;	/* size of cached list */
simple_lock_data_t	vm_cache_lock;	/* lock for object cache */

struct object_q	vm_object_list;		/* list of allocated objects */
long		vm_object_count;	/* count of all objects */
simple_lock_data_t	vm_object_list_lock;
					/* lock for object list and count */

vm_object_t	kernel_object;		/* the single kernel object */
vm_object_t	kmem_object;

#define	vm_object_cache_lock()		simple_lock(&vm_cache_lock)
#define	vm_object_cache_unlock()	simple_unlock(&vm_cache_lock)
#endif /* _KERNEL */

#define	vm_object_lock_init(object)	simple_lock_init(&(object)->Lock)
#define	vm_object_lock(object)		simple_lock(&(object)->Lock)
#define	vm_object_unlock(object)	simple_unlock(&(object)->Lock)
#define	vm_object_lock_try(object)	simple_lock_try(&(object)->Lock)

#define	vm_object_sleep(event, object, interruptible, where) \
	do {								\
		(object)->flags |= OBJ_WAITING;				\
		thread_sleep_msg((event), &(object)->Lock,		\
		    (interruptible), (where), 0);			\
	} while (0)

#define	vm_object_wakeup(object) \
	do {								\
		if ((object)->flags & OBJ_WAITING) {			\
			(object)->flags &= ~OBJ_WAITING;		\
			thread_wakeup((object));			\
		}							\
	} while (0)

#define	vm_object_paging(object) \
	((object)->paging_in_progress != 0)

#ifdef DIAGNOSTIC
/* Hope 0xdead is an unlikely value of either field */
#if BYTE_ORDER == BIG_ENDIAN
#define __VMOBJECT_IS_DEAD(object) \
	((object)->flags == 0xdead)
#else
#define __VMOBJECT_IS_DEAD(object) \
	((object)->paging_in_progress == 0xdead)
#endif /* BIG_ENDIAN */
#else
#define __VMOBJECT_IS_DEAD(object)	(0)
#endif /* DIAGNOSTIC */

#define	vm_object_paging_begin(object) \
	do {								\
		if (__VMOBJECT_IS_DEAD(object))				\
			panic("vm_object_paging_begin");		\
		(object)->paging_in_progress++;				\
	} while (0)

#define	vm_object_paging_end(object) \
	do {								\
		if (--((object)->paging_in_progress) == 0)		\
			vm_object_wakeup((object));			\
	} while (0)

#define	vm_object_paging_wait(object,msg) \
	do {								\
		while (vm_object_paging((object))) {			\
			vm_object_sleep((object), (object), FALSE,	\
			    (msg));					\
			vm_object_lock((object));			\
		}							\
	} while (0)

#ifdef _KERNEL
vm_object_t	 vm_object_allocate __P((vsize_t));
void		 vm_object_cache_clear __P((void));
void		 vm_object_cache_trim __P((void));
boolean_t	 vm_object_coalesce __P((vm_object_t, vm_object_t,
		    vaddr_t, vaddr_t, vaddr_t, vsize_t));
void		 vm_object_collapse __P((vm_object_t));
void		 vm_object_copy __P((vm_object_t, vaddr_t, vsize_t,
		    vm_object_t *, vaddr_t *, boolean_t *));
void		 vm_object_deactivate_pages __P((vm_object_t));
void		 vm_object_deallocate __P((vm_object_t));
void		 vm_object_enter __P((vm_object_t, vm_pager_t));
void		 vm_object_init __P((vsize_t));
vm_object_t	 vm_object_lookup __P((vm_pager_t));
boolean_t	 vm_object_page_clean __P((vm_object_t,
		    vaddr_t, vaddr_t, boolean_t, boolean_t));
void		 vm_object_page_remove __P((vm_object_t,
		    vaddr_t, vaddr_t));
void		 vm_object_pmap_copy __P((vm_object_t,
		    vaddr_t, vaddr_t));
void		 vm_object_pmap_remove __P((vm_object_t,
		    vaddr_t, vaddr_t));
void		 vm_object_prefer __P((vm_object_t,
		    vaddr_t, vaddr_t *));
void		 vm_object_print __P((vm_object_t, boolean_t));
void		 _vm_object_print __P((vm_object_t, boolean_t,
		    void (*)(const char *, ...)));
void		 vm_object_reference __P((vm_object_t));
void		 vm_object_remove __P((vm_pager_t));
void		 vm_object_setpager __P((vm_object_t,
		    vm_pager_t, vaddr_t, boolean_t));
void		 vm_object_shadow __P((vm_object_t *,
		    vaddr_t *, vsize_t));
#endif
#endif /* _VM_OBJECT_ */