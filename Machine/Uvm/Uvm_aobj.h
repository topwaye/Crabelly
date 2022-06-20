/* Machine: uvm_aobj.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_AOBJ_H_
#define _UVM_UVM_AOBJ_H_

/*
 * flags
 */

/* flags for uao_create: can only be used one time (at bootup) */
#define UAO_FLAG_KERNOBJ	0x1	/* create kernel object */
#define UAO_FLAG_KERNSWAP	0x2	/* enable kernel swap */

/* internal flags */
#define UAO_FLAG_KILLME		0x4	/* aobj should die when last released
					 * page is no longer PG_BUSY ... */
#define UAO_FLAG_NOSWAP		0x8	/* aobj can't swap (kernel obj only!) */

/*
 * prototypes
 */

int uao_set_swslot __P((struct uvm_object *, int, int));
void uao_dropswap __P((struct uvm_object *, int));

/*
 * globals
 */

extern struct uvm_pagerops aobj_pager;

#endif /* _UVM_UVM_AOBJ_H_ */