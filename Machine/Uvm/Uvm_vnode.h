/* Machine: uvm_vnode.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_VNODE_H_
#define _UVM_UVM_VNODE_H_

/*
 * uvm_vnode.h
 *
 * vnode handle into the VM system.
 */

/*
 * the uvm_vnode structure.   put at the top of the vnode data structure.
 * this allows:
 *   (struct vnode *) == (struct uvm_vnode *) == (struct uvm_object *)
 */

struct uvm_vnode {
	struct uvm_object u_obj;	/* the actual VM object */
	int u_flags;			/* flags */
	int u_nio;			/* number of running I/O requests */
	vsize_t u_size;		/* size of object */

	/* the following entry is locked by uvn_wl_lock */
	LIST_ENTRY(uvm_vnode) u_wlist;	/* list of writeable vnode objects */

	/* the following entry is locked by uvn_sync_lock */
	SIMPLEQ_ENTRY(uvm_vnode) u_syncq; /* vnode objects due for a "sync" */
};

/*
 * u_flags values
 */
#define UVM_VNODE_VALID		0x001	/* we are attached to the vnode */
#define UVM_VNODE_CANPERSIST	0x002	/* we can persist after ref == 0 */
#define UVM_VNODE_ALOCK		0x004	/* uvn_attach is locked out */
#define UVM_VNODE_DYING		0x008	/* final detach/terminate in 
					   progress */
#define UVM_VNODE_RELKILL	0x010	/* uvn should be killed by releasepg
					   when final i/o is done */
#define UVM_VNODE_WANTED	0x020	/* someone is waiting for alock,
					   dying, or relkill to clear */
#define UVM_VNODE_VNISLOCKED	0x040	/* underlying vnode struct is locked
					   (valid when DYING is true) */
#define UVM_VNODE_IOSYNC	0x080	/* I/O sync in progress ... setter
					   sleeps on &uvn->u_nio */
#define UVM_VNODE_IOSYNCWANTED	0x100	/* a process is waiting for the
					   i/o sync to clear so it can do
					   i/o */
#define UVM_VNODE_WRITEABLE	0x200	/* uvn has pages that are writeable */

/*
 * UVM_VNODE_BLOCKED: any condition that should new processes from
 * touching the vnode [set WANTED and sleep to wait for it to clear]
 */
#define UVM_VNODE_BLOCKED (UVM_VNODE_ALOCK|UVM_VNODE_DYING|UVM_VNODE_RELKILL)


/*
 * prototypes
 */

#if 0
/*
 * moved uvn_attach to uvm_extern.h because uvm_vnode.h is needed to
 * include sys/vnode.h, and files that include sys/vnode.h don't know
 * what a vm_prot_t is.
 */
struct uvm_object  *uvn_attach __P((void *, vm_prot_t));
#endif

#endif /* _UVM_UVM_VNODE_H_ */