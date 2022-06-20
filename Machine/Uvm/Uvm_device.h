/* Machine: uvm_device.h,v 1.7 2022/06/20 */

#ifndef _UVM_UVM_DEVICE_H_
#define _UVM_UVM_DEVICE_H_

/*
 * uvm_device.h
 *
 * device handle into the VM system.
 */

/*
 * the uvm_device structure.   object is put at the top of the data structure.
 * this allows:
 *   (struct uvm_device *) == (struct uvm_object *)
 */

struct uvm_device {
	struct uvm_object u_obj;	/* the actual VM object */
	int u_flags;			/* flags [LOCKED BY UDV_LOCK!] */
	dev_t u_device;		/* our device */
	LIST_ENTRY(uvm_device) u_list; /* list of device objects */
};

/*
 * u_flags values
 */

#define UVM_DEVICE_HOLD		0x1	/* someone has a "hold" on it */
#define UVM_DEVICE_WANTED	0x2	/* someone wants to put a "hold" on */

/*
 * prototypes
 */

struct uvm_object *udv_attach __P((void *, vm_prot_t, vaddr_t, vsize_t));

#endif /* _UVM_UVM_DEVICE_H_ */