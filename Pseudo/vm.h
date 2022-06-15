#ifndef VM_PSEUDO
#define VM_PSEUDO

TAILQ_HEAD(PGLIST, VMPAGE);
TAILQ_HEAD(HASHLIST, VMPAGE);
TAILQ_HEAD(MAPENTRYLIST, VMMAPENTRY);

struct VMPAGE
{
	boolean_t			copying;	/* PG_BUSY: page is being copied */
	count_t				refs;		/* reference count */
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
		count_t	offset;				/* offset into the main object */
		struct VMOBJECT* main;			/* object I point to */
		struct VMOBJECT* copy;			/* anonymous overlay */
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
