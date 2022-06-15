#ifndef VM_PSEUDO
#define VM_PSEUDO

TAILQ_HEAD(PGLIST, VMPAGE);
TAILQ_HEAD(HASHLIST, VMPAGE);
TAILQ_HEAD(MAPENTRYLIST, VMMAPENTRY);

struct VMOBJECT						/* there is no reference count */
{							/* cannot be held by other processes */
	count_t				pages;		/* number of pages in memq */
};

union VMMAPOBJECT
{
	VMMAP				submap;		/* belongs to another map */
	struct
	{	
		count_t	offset;				/* offset into the main object */
		struct VMOBJECT	main;			/* doesn't allow to hold another object */
	}				obj;
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
	simple_lock_data_t		lock;		/* lock for the entire treemap */
	struct VMMAP			map;		/* virtual map */
	struct PHYSMAP*			pmap;		/* physical map */
};

#endif
