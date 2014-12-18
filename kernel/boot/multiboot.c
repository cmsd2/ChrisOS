
#include <boot/multiboot.h>
#include <utils/kprintf.h>
#include <sys/multiboot.h>
#include <utils/mem.h>
#include <mm/layout.h>
#include <mm/kmem.h>
#include <assert.h>
#include <string.h>

uint32_t _magic;
uintptr_t _bootstrap_multiboot_info;

struct multiboot_info _multiboot_info;

void load_multiboot_info(void) {
	if(_magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Found multiboot magic number\n");
	} else {
		kprintf("Invalid multiboot magic number\n");
		return;
	}

	struct multiboot_info * _bootstrap_info = (struct multiboot_info *) (_bootstrap_multiboot_info + _kernel_layout.segment_start);

	kprintf("Loading multiboot info from 0x%x\n", _bootstrap_info);

	memset(&_multiboot_info, 0, sizeof(_multiboot_info));

	_multiboot_info.flags = _bootstrap_info->flags;

	if(_multiboot_info.flags & MULTIBOOT_INFO_MEMORY) {
		_multiboot_info.mem_lower = _bootstrap_info->mem_lower;
		_multiboot_info.mem_upper = _bootstrap_info->mem_upper;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_BOOTDEV) {
		_multiboot_info.boot_device = _bootstrap_info->boot_device;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_CMDLINE) {
        _multiboot_info.cmdline = multiboot_relocate_str(_bootstrap_info->cmdline);
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_MODS) {
		_multiboot_info.mods_count = _bootstrap_info->mods_count;
        _multiboot_info.mods_addr = multiboot_relocate_mem(_bootstrap_info->mods_addr, _bootstrap_info->mods_count * sizeof(struct multiboot_mod_list));
        
		struct multiboot_mod_list * mods = (struct multiboot_mod_list *) (uintptr_t) _multiboot_info.mods_addr;
		for(unsigned int i = 0; i < _multiboot_info.mods_count; i++) {
            size_t mod_size = mods[i].mod_end - mods[i].mod_start;
            mods[i].mod_start = multiboot_relocate_mem(mods[i].mod_start, mod_size);
            mods[i].mod_end = mods[i].mod_start + mod_size;
		}
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_AOUT_SYMS) {
		_multiboot_info.u.aout_sym.addr += _kernel_layout.segment_start;
	} else if(_multiboot_info.flags & MULTIBOOT_INFO_ELF_SHDR) {
		_multiboot_info.u.elf_sec.addr += _kernel_layout.segment_start;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_MEM_MAP) {
		_multiboot_info.mmap_length = _bootstrap_info->mmap_length;
        _multiboot_info.mmap_addr = multiboot_relocate_mem(_bootstrap_info->mmap_addr, _bootstrap_info->mmap_length);

		//struct multiboot_mmap_entry * mmap = (struct multiboot_mmap_entry *) _multiboot_info.mmap_addr;

		//for(unsigned int i = 0; i < (_multiboot_info.mmap_length / sizeof(struct multiboot_mmap_entry)); i++) {
			
		//}
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_DRIVE_INFO) {
		_multiboot_info.drives_length = _bootstrap_info->drives_length;
		_multiboot_info.drives_addr = _bootstrap_info->drives_addr;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_CONFIG_TABLE) {
		_multiboot_info.config_table = _bootstrap_info->config_table;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
		_multiboot_info.boot_loader_name = _bootstrap_info->boot_loader_name;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_APM_TABLE) {
		_multiboot_info.apm_table = _bootstrap_info->apm_table;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_VBE_INFO) {
		_multiboot_info.vbe_control_info = _bootstrap_info->vbe_control_info;
		_multiboot_info.vbe_mode_info = _bootstrap_info->vbe_mode_info;
		_multiboot_info.vbe_mode = _bootstrap_info->vbe_mode;
		_multiboot_info.vbe_interface_seg = _bootstrap_info->vbe_interface_seg;
		_multiboot_info.vbe_interface_off = _bootstrap_info->vbe_interface_off;
		_multiboot_info.vbe_interface_len = _bootstrap_info->vbe_interface_len;
	}

	if(_multiboot_info.flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
		_multiboot_info.framebuffer_addr = _bootstrap_info->framebuffer_addr;
		_multiboot_info.framebuffer_pitch = _bootstrap_info->framebuffer_pitch;
		_multiboot_info.framebuffer_width = _bootstrap_info->framebuffer_width;
		_multiboot_info.framebuffer_height = _bootstrap_info->framebuffer_height;
		_multiboot_info.framebuffer_bpp = _bootstrap_info->framebuffer_bpp;
		_multiboot_info.framebuffer_type = _bootstrap_info->framebuffer_type;
		_multiboot_info.framebuffer_colors_u = _bootstrap_info->framebuffer_colors_u;
	}
}

uintptr_t multiboot_relocate_mem(uintptr_t physaddr, size_t size) {
    uintptr_t vaddr = physaddr + _kernel_layout.segment_start;
    uintptr_t result = kalloc_dup_static(vaddr, size, sizeof(uintptr_t));
    return result;
}

uintptr_t multiboot_relocate_str(uintptr_t physaddr) {
    uintptr_t vaddr = physaddr + _kernel_layout.segment_start;
    size_t size = strlen((char*)vaddr) + 1;
    uintptr_t result = kalloc_dup_static(vaddr, size, sizeof(int));
    return result;
}

void multiboot_copy_mem_map_to_allocator(void) {
	assert(_multiboot_info.flags & MULTIBOOT_INFO_MEM_MAP);

	uintptr_t mmap_end = _multiboot_info.mmap_addr + _multiboot_info.mmap_length;
	uintptr_t mmap_cur = _multiboot_info.mmap_addr;
	int i = 0;
	enum alloc_region_flags flags = ALLOC_PM_NORMAL;
	while(mmap_cur < mmap_end) {
		struct multiboot_mmap_entry * mmap = (struct multiboot_mmap_entry *) mmap_cur;
		if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			kern_pm_alloc_region_add(mmap->addr, mmap->len, flags);
		}
		i++;
		mmap_cur += mmap->size + sizeof(mmap->size);
	}
}



void multiboot_print_info(void) {
	multiboot_print_flags();
	multiboot_print_mem_info();
	multiboot_print_bootdev_info();
	multiboot_print_cmdline_info();
	multiboot_print_mods_info();
	multiboot_print_aout_syms_info();
	multiboot_print_elf_shdr_info();
	multiboot_print_mem_map_info();
	multiboot_print_drive_info();
	multiboot_print_config_table_info();
	multiboot_print_boot_loader_name_info();
	multiboot_print_apm_table_info();
	multiboot_print_vbe_info();
	multiboot_print_framebuffer_info();
}

void multiboot_print_flags(void) {
	#define F2I(f) ((_multiboot_info.flags & MULTIBOOT_INFO_ ## f ) ? 1 : 0)
	kprintf("memory=%d bootdev=%d cmdline=%d mods=%d aout_syms=%d elf_shdr=%d mem_map=%d drive_info=%d config_table=%d boot_loader_name=%d apm_table=%d vbe_info=%d framebuffer_info=%d\n",
		F2I(MEMORY),
		F2I(BOOTDEV),
		F2I(CMDLINE),
		F2I(MODS),
		F2I(AOUT_SYMS),
		F2I(ELF_SHDR),
		F2I(MEM_MAP),
		F2I(DRIVE_INFO),
		F2I(CONFIG_TABLE),
		F2I(BOOT_LOADER_NAME),
		F2I(APM_TABLE),
		F2I(VBE_INFO),
		F2I(FRAMEBUFFER_INFO)
	);
}

void multiboot_print_mem_info(void) {
	kprintf("mem_lower=0x%x mem_upper=0x%x\n", _multiboot_info.mem_lower, _multiboot_info.mem_upper);	
}

void multiboot_print_bootdev_info(void) {
	kprintf("boot_device=0x%x\n", _multiboot_info.boot_device);
}

void multiboot_print_cmdline_info(void) {
	if(_multiboot_info.flags & MULTIBOOT_INFO_CMDLINE) {
		kprintf("cmdline=0x%x cmdline_str=%s\n", _multiboot_info.cmdline, _multiboot_info.cmdline);
	} else {
		kprintf("cmdline=\n");
	}
}

void multiboot_print_mods_info(void) {
	kprintf("mods_count=%u mods_addr=0x%x\n", _multiboot_info.mods_count, _multiboot_info.mods_addr);

	struct multiboot_mod_list * mod = (struct multiboot_mod_list *) (uintptr_t) _multiboot_info.mods_addr;
	for(unsigned int i = 0; i < _multiboot_info.mods_count; i++) {
		kprintf("mod_%d_start=0x%x mod_%d_end=0x%x mod_%d_str=%s\n",
			i, mod[i].mod_start,
			i, mod[i].mod_end,
			i, mod[i].cmdline);
	}
}

void multiboot_print_aout_syms_info(void) {
	if(_multiboot_info.flags & MULTIBOOT_INFO_AOUT_SYMS) {
		kprintf("aout_sym_addr=0x%x\n", _multiboot_info.u.aout_sym.addr);
	}
}

void multiboot_print_elf_shdr_info(void) {
	if(_multiboot_info.flags & MULTIBOOT_INFO_AOUT_SYMS) {
		kprintf("elf_sec_addr=0x%x\n", _multiboot_info.u.elf_sec.addr);
	}
}

void multiboot_print_mem_map_info(void) {
	//struct multiboot_mmap_entry * m = (struct multiboot_mmap_entry *)
	if(_multiboot_info.flags & MULTIBOOT_INFO_MEM_MAP) {
		kprintf("mmap_length=0x%x mmap_addr=0x%x\n", _multiboot_info.mmap_length, _multiboot_info.mmap_addr);

		uintptr_t mmap_end = _multiboot_info.mmap_addr + _multiboot_info.mmap_length;
		uintptr_t mmap_cur = _multiboot_info.mmap_addr;
		int i = 0;
		while(mmap_cur < mmap_end) {
			struct multiboot_mmap_entry * mmap = (struct multiboot_mmap_entry *) mmap_cur;
			kprintf("mmap_%d_ptr=0x%x mmap_%d_size=0x%x mmap_%d_addr=0x%llx mmap_%d_len=0x%llx mmap_%d_type=%d\n",
				i, mmap,
				i, mmap->size, 
				i, mmap->addr, 
				i, mmap->len, 
				i, mmap->type);
			i++;
			mmap_cur += mmap->size + sizeof(mmap->size);
		}
	}
}

void multiboot_print_drive_info(void) {
}

void multiboot_print_config_table_info(void) {
}

void multiboot_print_boot_loader_name_info(void) {
}

void multiboot_print_apm_table_info(void) {
}

void multiboot_print_vbe_info(void) {
}

void multiboot_print_framebuffer_info(void) {
}

const char * multiboot_get_cmdline(void) {
    return (const char *)_multiboot_info.cmdline;
}
