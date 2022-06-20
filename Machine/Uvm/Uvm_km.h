/* Machine: uvm_km.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_KM_H_
#define _UVM_UVM_KM_H_

/*
 * uvm_km.h
 */

/*
 * prototypes
 */

void uvm_km_init __P((vaddr_t, vaddr_t));
void uvm_km_pgremove __P((struct uvm_object *, vaddr_t, vaddr_t));

#endif /* _UVM_UVM_KM_H_ */