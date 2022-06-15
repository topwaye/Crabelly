#ifndef VM_PSEUDO
#define VM_PSEUDO

struct VMSEGMENT
{
	count_t				offset;		/* offset into pager */
	count_t				copyings;	/* number of copyings */
	vaddr_t				copying[16];	/* pages being copied */
};

union VMMAPSEG
{
	VMMAP				submap;		/* belongs to another map */
	VMSEGMENT			segment;
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
