#ifndef VM_PSEUDO
#define VM_PSEUDO

TAILQ_HEAD(PGLIST, vaddr_t); 

struct VMSEGMENT							/* pages do exist, but */
{									/* VMPAGE struct does NOT exist */
	count_t			offset;					/* offset into pager */
	struct PGLIST		copyingq;				/* pages being copied, more than CPU count */
};

union VMMAPSEG
{
	VMMAP			submap;					/* belongs to another map */
	VMSEGMENT		segment;
};

struct VMMAPENTRY
{
	vaddr_t			start;					/* start address */
	vaddr_t			end;					/* end address */
	union VMMAPSEG		mapseg;
};

struct VMMAP
{
	struct MAPENTRYLIST	entryq;					/* list of entries */
	count_t			entries;				/* number of entries */
};

struct VMSPACE
{
	simple_lock_data_t	lock;					/* lock for the entire treemap */
	struct VMMAP		map;					/* virtual map */
	struct PHYSMAP*		pmap;					/* physical map */
};

#endif
