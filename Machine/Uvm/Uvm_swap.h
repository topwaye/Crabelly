/* Machine: uvm_swap.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_SWAP_H_
#define _UVM_UVM_SWAP_H_

int			uvm_swap_get __P((struct vm_page *, int, int));
int			uvm_swap_put __P((int, struct vm_page **, int,
			    int));
int			uvm_swap_alloc __P((int *wanted, boolean_t lessok));
void			uvm_swap_free __P((int startslot, int nslots));

#endif /* _UVM_UVM_SWAP_H_ */