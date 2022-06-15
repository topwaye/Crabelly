#ifndef VM_PSEUDO
#define VM_PSEUDO

TAILQ_HEAD(PGLIST, VMPAGE);
TAILQ_HEAD(HASHLIST, VMPAGE);
TAILQ_HEAD(MAPENTRYLIST, VMMAPENTRY);

union VMMAPSEG
{
	VMMAP				submap;		/* belongs to another map */
	struct
	{	
		count_t	offset;				/* offset into src */
	}				segment;
};

struct VMMAPENTRY
{
	vaddr_t				start;		/* start address */
	vaddr_t				end;		/* end address */
	union VMMAPSEG			mapseg;
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
