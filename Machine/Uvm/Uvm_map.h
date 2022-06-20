/* Machine: uvm_map.h,v 1.00 2022/06/20 */

#ifndef _UVM_UVM_MAP_H_
#define _UVM_UVM_MAP_H_

/*
 * uvm_map.h
 */

/*
 * macros
 */

/*
 * UVM_MAP_CLIP_START: ensure that the entry begins at or after
 * the starting address, if it doesn't we split the entry.
 * 
 * => map must be locked by caller
 */

#define UVM_MAP_CLIP_START(MAP,ENTRY,VA) { \
	if ((VA) > (ENTRY)->start) uvm_map_clip_start(MAP,ENTRY,VA); }

/*
 * UVM_MAP_CLIP_END: ensure that the entry ends at or before
 *      the ending address, if it does't we split the entry.
 *
 * => map must be locked by caller
 */

#define UVM_MAP_CLIP_END(MAP,ENTRY,VA) { \
	if ((VA) < (ENTRY)->end) uvm_map_clip_end(MAP,ENTRY,VA); }

/*
 * extract flags
 */
#define UVM_EXTRACT_REMOVE	0x1	/* remove mapping from old map */
#define UVM_EXTRACT_CONTIG	0x2	/* try to keep it contig */
#define UVM_EXTRACT_QREF	0x4	/* use quick refs */
#define UVM_EXTRACT_FIXPROT	0x8	/* set prot to maxprot as we go */


/*
 * handle inline options
 */

#ifdef UVM_MAP_INLINE
#define MAP_INLINE static __inline
#else 
#define MAP_INLINE /* nothing */
#endif /* UVM_MAP_INLINE */

/*
 * protos: the following prototypes define the interface to vm_map
 */

MAP_INLINE
void		uvm_map_deallocate __P((vm_map_t));

int		uvm_map_clean __P((vm_map_t, vaddr_t, vaddr_t, int));
void		uvm_map_clip_start __P((vm_map_t,
				vm_map_entry_t, vaddr_t));
void		uvm_map_clip_end __P((vm_map_t, vm_map_entry_t,
				vaddr_t));
MAP_INLINE
vm_map_t	uvm_map_create __P((pmap_t, vaddr_t, 
			vaddr_t, boolean_t));
int		uvm_map_extract __P((vm_map_t, vaddr_t, vsize_t, 
			vm_map_t, vaddr_t *, int));
vm_map_entry_t	uvm_map_findspace __P((vm_map_t, vaddr_t, vsize_t,
			vaddr_t *, struct uvm_object *, vaddr_t, 
			boolean_t));
int		uvm_map_inherit __P((vm_map_t, vaddr_t, vaddr_t,
			vm_inherit_t));
void		uvm_map_init __P((void));
boolean_t	uvm_map_lookup_entry __P((vm_map_t, vaddr_t, 
			vm_map_entry_t *));
MAP_INLINE
void		uvm_map_reference __P((vm_map_t));
int		uvm_map_replace __P((vm_map_t, vaddr_t, vaddr_t, 
			vm_map_entry_t, int));
int		uvm_map_reserve __P((vm_map_t, vsize_t, vaddr_t, 
			vaddr_t *));
void		uvm_map_setup __P((vm_map_t, vaddr_t, 
			vaddr_t, boolean_t));
int		uvm_map_submap __P((vm_map_t, vaddr_t, 
			vaddr_t, vm_map_t));
MAP_INLINE
int		uvm_unmap __P((vm_map_t, vaddr_t, vaddr_t));
void		uvm_unmap_detach __P((vm_map_entry_t,int));
int		uvm_unmap_remove __P((vm_map_t, vaddr_t, vaddr_t,
				      vm_map_entry_t *));

struct vmspace *uvmspace_fork __P((struct vmspace *));

#endif /* _UVM_UVM_MAP_H_ */