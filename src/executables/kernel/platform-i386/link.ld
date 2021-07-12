ENTRY(_start)

SECTIONS {
	.text 0x100000 : {
		kernel_start = .;
		kernel_code_start = .;
		KEEP(*(.mboot))
		*(.text)
		kernel_code_end = .;
	}

	.rodata ALIGN(4) : {
		kernel_rodata_start = .;
		*(.rodata)
		kernel_rodata_end = .;
	}

	.data ALIGN(4) : {
		__CTOR_LIST__ = .;
		*(.ctors)
		*(.init_array)
		LONG(0)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		*(.dtors)
		*(.fini_array)
		LONG(0)
		__DTOR_END__ = .;

		kernel_data_start = .;
		*(.data)
		kernel_data_end = .;
	}

	.bss ALIGN(4) : {
		kernel_bss_start = .;
		*(COMMON)
		*(.bss)
		kernel_bss_end = .;
	}

	.comment ALIGN(4) : {
		kernel_comment_start = .;
		*(.comment)
		kernel_comment_end = .;
	}

	.debug_info ALIGN(4) : {
		kernel_debug_info_start = .;
		*(.debug_info)
		kernel_debug_info_end = .;

		kernel_debug_str_start = .;
		*(.debug_str)
		kernel_debug_str_end = .;

		kernel_debug_abbrev_start = .;
		*(.debug_abbrev)
		kernel_debug_abbrev_end = .;

		kernel_debug_macinfo_start = .;
		*(.debug_macinfo)
		kernel_debug_macinfo_end = .;

		kernel_debug_pubnames_start = .;
		*(.debug_pubnames)
		kernel_debug_pubnames_end = .;

		kernel_debug_pubtypes_start = .;
		*(.debug_pubtypes)
		kernel_debug_pubtypes_end = .;

		kernel_debug_frame_start = .;
		*(.debug_frame)
		kernel_debug_frame_end = .;

		kernel_debug_line_start = .;
		*(.debug_line)
		kernel_debug_line_end = .;

		kernel_debug_loc_start = .;
		*(.debug_loc)
		kernel_debug_loc_end = .;
	}

	kernel_end = .;
}

