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

TAILQ_HEAD(PGLIST, VMPAGE);
TAILQ_HEAD(HASHLIST, VMPAGE);
TAILQ_HEAD(MAPENTRYLIST, VMMAPENTRY);

struct VMPAGE
{
	vaddr_t				offset;		/* offset into object */
	paddr_t				physaddr;	/* physical address of page */
};

struct VMOBJECT
{	
	struct HASHLIST			hashq;		/* hash table links */
	struct PGLIST			memq;		/* pages in this object */
	count_t				pages;		/* number of pages in memq */
	count_t				refs;		/* reference count */
};

union VMMAPOBJECT
{
	VMMAP				submap;		/* belongs to another map */
	struct
	{	
		vsize_t	offset;				/* offset into the main object */
		struct VMOBJECT	main;			/* object I point to */
		struct VMOBJECT	copy;			/* anonymous overlay */
	}				object;
};

struct VMMAPENTRY
{
	vaddr_t				start;		/* start address */
	vaddr_t				end;		/* end address */
	union VMMAPOBJECT		mapobj;
};

struct VMMAP
{
	struct MAPENTRYLIST		entryq;		/* list of entries */
	count_t				entries;	/* number of entries */
};

struct VMSPACE
{
	struct VMMAP			map;		/* virtual map */
	struct PHYSMAP*			pmap;		/* physical map */
};

#endif
