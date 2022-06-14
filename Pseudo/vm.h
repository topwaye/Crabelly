#ifndef VM_PSEUDO
#define VM_PSEUDO

typedef unsigned long	count_t;
typedef unsigned long	vaddr_t;
typedef unsigned long	vsize_t;
typedef unsigned long	paddr_t;

/* Tail queue definitions */
#define TAILQ_HEAD(name, type)							\
struct name 									\
{										\
	struct type *tqh_first;	/* first element */				\
	struct type **tqh_last;	/* addr of last next element */			\
}

#define TAILQ_ENTRY(type)							\
struct										\
{										\
	struct type *tqe_next;	/* next element */				\
	struct type **tqe_prev;	/* address of previous next element */		\
}

TAILQ_HEAD(PGLIST, VMPAGE);

struct VMPAGE
{
	TAILQ_ENTRY(VMPAGE)		pageq;		/* queue info for FIFO queue or free list */
	TAILQ_ENTRY(VMPAGE)		hashq;		/* hash table links */
	TAILQ_ENTRY(VMPAGE)		listq;		/* pages in same object */
	
	vaddr_t				offset;		/* offset into object */
	struct UVMOBJECT*		object;		/* which object am I in */
	struct VMANON*			anon;		/* which anon am I in */
	count_t				wired;		/* wired down map refs */
	count_t				loans;		/* number of active loans */
	paddr_t				physaddr;	/* physical address of page */
};

struct UVMOBJECT
{
	struct PGLIST			memq;		/* pages in this object */
	count_t				pages;		/* number of pages in memq */
	count_t				refs;		/* reference count */
};

union VMMAPOBJECT
{
	struct UVMOBJECT*		obj;
	struct VMMAP*			submap;		/* belongs to another map */
};

struct VMMAPENTRY
{
	struct VMMAPENTRY*		prev;
	struct VMMAPENTRY*		next;
	vaddr_t				start;		/* start address */
	vaddr_t				end;		/* end address */
	union VMMAPOBJECT		object;		/* object I point to */
	vsize_t				offset;		/* offset into object */
	count_t				wired;		/* can be paged if = 0 */
	struct VMAREF			aref;		/* anonymous overlay */
};

struct VMMAP
{
	struct PHYSMAP*			physmap;	/* physical map */
	struct VMMAPENTRY		header;		/* list of entries */
	count_t				entries;	/* number of entries */
};

struct VMSPACE
{
	struct VMMAP			map;		/* virtual map */
};

#endif
