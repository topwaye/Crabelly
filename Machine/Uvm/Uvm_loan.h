/* Machine: uvm_loan.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_LOAN_H_
#define _UVM_UVM_LOAN_H_

/*
 * flags for uvm_loan
 */

#define UVM_LOAN_TOANON		0x1		/* loan to anon */
#define UVM_LOAN_TOPAGE		0x2		/* loan to page */

/*
 * loan prototypes
 */

int uvm_loan __P((struct vm_map *, vaddr_t, vsize_t, void **, int));
void uvm_unloananon __P((struct vm_anon **, int));
void uvm_unloanpage __P((struct vm_page **, int));

#endif /* _UVM_UVM_LOAN_H_ */