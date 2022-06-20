/* Machine: uvm_ddb.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_DDB_H_
#define _UVM_UVM_DDB_H_

#if defined(DDB)
void			uvm_map_print __P((vm_map_t, boolean_t));
void			uvm_map_printit __P((vm_map_t, boolean_t,
				void (*) __P((const char *, ...))));

void			uvm_object_print __P((struct uvm_object *, boolean_t));
void			uvm_object_printit __P((struct uvm_object *, boolean_t,
				void (*) __P((const char *, ...))));
void			uvm_page_print __P((struct vm_page *, boolean_t));
void			uvm_page_printit __P((struct vm_page *, boolean_t,
				void (*) __P((const char *, ...))));
#endif
#endif _UVM_UVM_DDB_H_