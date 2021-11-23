	.text
	.file	"simple_stencil.cc"
	.globl	_Z13basicSstenciliiPKfPf        # -- Begin function _Z13basicSstenciliiPKfPf
	.p2align	4, 0x90
	.type	_Z13basicSstenciliiPKfPf,@function
_Z13basicSstenciliiPKfPf:               # @_Z13basicSstenciliiPKfPf
.Lfunc_begin0:
	.file	1 "/home/icohedron/workspace/KernelFaRer" "simple_stencil.cc"
	.loc	1 2 0                           # simple_stencil.cc:2:0
	.cfi_startproc
# %bb.0:                                # %entry
	#DEBUG_VALUE: basicSstencil:imin <- $edi
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r12
	.cfi_def_cfa_offset 32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset %rbx, -40
	.cfi_offset %r12, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
.Ltmp0:
	#DEBUG_VALUE: i <- $edi
                                        # kill: def $esi killed $esi def $rsi
	#DEBUG_VALUE: i <- $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	#DEBUG_VALUE: basicSstencil:imin <- $edi
	.loc	1 3 23 prologue_end             # simple_stencil.cc:3:23
	cmpl	%esi, %edi
.Ltmp1:
	.loc	1 3 2 is_stmt 0                 # simple_stencil.cc:3:2
	jg	.LBB0_16
.Ltmp2:
# %bb.1:                                # %for.body.lr.ph
	#DEBUG_VALUE: i <- $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	#DEBUG_VALUE: basicSstencil:imin <- $edi
	movslq	%edi, %r11
	movslq	%esi, %r8
	leaq	1(%r8), %r15
	movq	%r15, %r9
	subq	%r11, %r9
	cmpq	$8, %r9
	jb	.LBB0_12
.Ltmp3:
# %bb.2:                                # %vector.memcheck
	#DEBUG_VALUE: i <- $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	#DEBUG_VALUE: basicSstencil:imin <- $edi
	leaq	(%rcx,%r11,4), %rax
	leaq	(%rdx,%r8,4), %rdi
.Ltmp4:
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	addq	$8, %rdi
	cmpq	%rdi, %rax
	jae	.LBB0_4
.Ltmp5:
# %bb.3:                                # %vector.memcheck
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	leaq	(%rcx,%r8,4), %rax
	addq	$4, %rax
	leaq	(%rdx,%r11,4), %rdi
	addq	$-4, %rdi
	cmpq	%rax, %rdi
	jb	.LBB0_12
.Ltmp6:
.LBB0_4:                                # %vector.ph
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	movq	%r9, %r10
	andq	$-8, %r10
	leaq	-8(%r10), %rax
	movq	%rax, %r14
	shrq	$3, %r14
	addq	$1, %r14
	testq	%rax, %rax
	je	.LBB0_5
.Ltmp7:
# %bb.6:                                # %vector.ph.new
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	leaq	(%rcx,%r11,4), %r12
	addq	$48, %r12
	leaq	(%rdx,%r11,4), %rax
	addq	$52, %rax
	movq	%r14, %rbx
	andq	$-2, %rbx
	negq	%rbx
	xorl	%edi, %edi
.Ltmp8:
	.p2align	4, 0x90
.LBB0_7:                                # %vector.body
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 4 10 is_stmt 1                # simple_stencil.cc:4:10
	movups	-56(%rax,%rdi,4), %xmm0
	.loc	1 4 19 is_stmt 0                # simple_stencil.cc:4:19
	movups	-48(%rax,%rdi,4), %xmm1
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm1
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movups	-40(%rax,%rdi,4), %xmm0
	.loc	1 4 19                          # simple_stencil.cc:4:19
	movups	-32(%rax,%rdi,4), %xmm2
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm2
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movups	%xmm1, -48(%r12,%rdi,4)
	movups	%xmm2, -32(%r12,%rdi,4)
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movups	-24(%rax,%rdi,4), %xmm0
	.loc	1 4 19                          # simple_stencil.cc:4:19
	movups	-16(%rax,%rdi,4), %xmm1
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm1
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movups	-8(%rax,%rdi,4), %xmm0
	.loc	1 4 19                          # simple_stencil.cc:4:19
	movups	(%rax,%rdi,4), %xmm2
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm2
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movups	%xmm1, -16(%r12,%rdi,4)
	movups	%xmm2, (%r12,%rdi,4)
	addq	$16, %rdi
	addq	$2, %rbx
	jne	.LBB0_7
.Ltmp9:
# %bb.8:                                # %middle.block.unr-lcssa
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 0 8                           # simple_stencil.cc:0:8
	testb	$1, %r14b
	je	.LBB0_10
.Ltmp10:
.LBB0_9:                                # %vector.body.epil
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	addq	%r11, %rdi
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movups	-4(%rdx,%rdi,4), %xmm0
	.loc	1 4 19                          # simple_stencil.cc:4:19
	movups	4(%rdx,%rdi,4), %xmm1
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm1
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movups	12(%rdx,%rdi,4), %xmm0
	.loc	1 4 19                          # simple_stencil.cc:4:19
	movups	20(%rdx,%rdi,4), %xmm2
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addps	%xmm0, %xmm2
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movups	%xmm1, (%rcx,%rdi,4)
	movups	%xmm2, 16(%rcx,%rdi,4)
.Ltmp11:
.LBB0_10:                               # %middle.block
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 3 2 is_stmt 1                 # simple_stencil.cc:3:2
	cmpq	%r10, %r9
	je	.LBB0_16
.Ltmp12:
# %bb.11:
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 0 2 is_stmt 0                 # simple_stencil.cc:0:2
	addq	%r10, %r11
.Ltmp13:
.LBB0_12:                               # %for.body.preheader
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 3 2                           # simple_stencil.cc:3:2
	subl	%r11d, %esi
.Ltmp14:
	#DEBUG_VALUE: basicSstencil:imax <- [DW_OP_LLVM_entry_value 1] $esi
	addl	$1, %esi
	movq	%r11, %rax
	testb	$1, %sil
	je	.LBB0_14
.Ltmp15:
# %bb.13:                               # %for.body.prol
	#DEBUG_VALUE: basicSstencil:imax <- [DW_OP_LLVM_entry_value 1] $esi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: i <- $r11
	.loc	1 4 10 is_stmt 1                # simple_stencil.cc:4:10
	movss	-4(%rdx,%r11,4), %xmm0          # xmm0 = mem[0],zero,zero,zero
	.loc	1 4 22 is_stmt 0                # simple_stencil.cc:4:22
	leaq	1(%r11), %rax
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addss	4(%rdx,%r11,4), %xmm0
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movss	%xmm0, (%rcx,%r11,4)
.Ltmp16:
	#DEBUG_VALUE: i <- $rax
.LBB0_14:                               # %for.body.prol.loopexit
	#DEBUG_VALUE: basicSstencil:imax <- [DW_OP_LLVM_entry_value 1] $esi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	.loc	1 3 2 is_stmt 1                 # simple_stencil.cc:3:2
	cmpq	%r8, %r11
	je	.LBB0_16
.Ltmp17:
	.p2align	4, 0x90
.LBB0_15:                               # %for.body
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: basicSstencil:imax <- [DW_OP_LLVM_entry_value 1] $esi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: i <- $rax
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movss	-4(%rdx,%rax,4), %xmm0          # xmm0 = mem[0],zero,zero,zero
	.loc	1 4 17 is_stmt 0                # simple_stencil.cc:4:17
	addss	4(%rdx,%rax,4), %xmm0
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movss	%xmm0, (%rcx,%rax,4)
.Ltmp18:
	#DEBUG_VALUE: i <- [DW_OP_plus_uconst 1, DW_OP_stack_value] $rax
	.loc	1 4 10                          # simple_stencil.cc:4:10
	movss	(%rdx,%rax,4), %xmm0            # xmm0 = mem[0],zero,zero,zero
	.loc	1 4 17                          # simple_stencil.cc:4:17
	addss	8(%rdx,%rax,4), %xmm0
	.loc	1 4 8                           # simple_stencil.cc:4:8
	movss	%xmm0, 4(%rcx,%rax,4)
.Ltmp19:
	#DEBUG_VALUE: i <- undef
	.loc	1 4 22                          # simple_stencil.cc:4:22
	leaq	2(%rax), %rsi
	movq	%rsi, %rax
.Ltmp20:
	.loc	1 3 23 is_stmt 1                # simple_stencil.cc:3:23
	cmpq	%rsi, %r15
.Ltmp21:
	.loc	1 3 2 is_stmt 0                 # simple_stencil.cc:3:2
	jne	.LBB0_15
.Ltmp22:
.LBB0_16:                               # %for.cond.cleanup
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	.loc	1 6 1 is_stmt 1                 # simple_stencil.cc:6:1
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	retq
.Ltmp23:
.LBB0_5:
	.cfi_def_cfa_offset 40
	#DEBUG_VALUE: basicSstencil:imin <- [DW_OP_LLVM_entry_value 1] $edi
	#DEBUG_VALUE: basicSstencil:B <- $rcx
	#DEBUG_VALUE: basicSstencil:A <- $rdx
	#DEBUG_VALUE: basicSstencil:imax <- $esi
	.loc	1 0 1 is_stmt 0                 # simple_stencil.cc:0:1
	xorl	%edi, %edi
	testb	$1, %r14b
	jne	.LBB0_9
	jmp	.LBB0_10
.Lfunc_end0:
	.size	_Z13basicSstenciliiPKfPf, .Lfunc_end0-_Z13basicSstenciliiPKfPf
	.cfi_endproc
                                        # -- End function
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
	.quad	.Lfunc_begin0-.Lfunc_begin0
	.quad	.Ltmp4-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	85                              # super-register DW_OP_reg5
	.quad	.Ltmp4-.Lfunc_begin0
	.quad	.Ltmp13-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	85                              # super-register DW_OP_reg5
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp23-.Lfunc_begin0
	.quad	.Lfunc_end0-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	85                              # super-register DW_OP_reg5
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
.Ldebug_loc1:
	.quad	.Lfunc_begin0-.Lfunc_begin0
	.quad	.Ltmp14-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	84                              # super-register DW_OP_reg4
	.quad	.Ltmp14-.Lfunc_begin0
	.quad	.Ltmp22-.Lfunc_begin0
	.short	4                               # Loc expr size
	.byte	243                             # DW_OP_GNU_entry_value
	.byte	1                               # 1
	.byte	84                              # super-register DW_OP_reg4
	.byte	159                             # DW_OP_stack_value
	.quad	.Ltmp23-.Lfunc_begin0
	.quad	.Lfunc_end0-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	84                              # super-register DW_OP_reg4
	.quad	0
	.quad	0
.Ldebug_loc2:
	.quad	.Ltmp0-.Lfunc_begin0
	.quad	.Ltmp4-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	85                              # super-register DW_OP_reg5
	.quad	.Ltmp15-.Lfunc_begin0
	.quad	.Ltmp16-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	91                              # DW_OP_reg11
	.quad	.Ltmp17-.Lfunc_begin0
	.quad	.Ltmp18-.Lfunc_begin0
	.short	1                               # Loc expr size
	.byte	80                              # DW_OP_reg0
	.quad	.Ltmp18-.Lfunc_begin0
	.quad	.Ltmp19-.Lfunc_begin0
	.short	3                               # Loc expr size
	.byte	112                             # DW_OP_breg0
	.byte	1                               # 1
	.byte	159                             # DW_OP_stack_value
	.quad	0
	.quad	0
	.section	.debug_abbrev,"",@progbits
	.byte	1                               # Abbreviation Code
	.byte	17                              # DW_TAG_compile_unit
	.byte	1                               # DW_CHILDREN_yes
	.byte	37                              # DW_AT_producer
	.byte	14                              # DW_FORM_strp
	.byte	19                              # DW_AT_language
	.byte	5                               # DW_FORM_data2
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	16                              # DW_AT_stmt_list
	.byte	23                              # DW_FORM_sec_offset
	.byte	27                              # DW_AT_comp_dir
	.byte	14                              # DW_FORM_strp
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	2                               # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.ascii	"\227B"                         # DW_AT_GNU_all_call_sites
	.byte	25                              # DW_FORM_flag_present
	.byte	110                             # DW_AT_linkage_name
	.byte	14                              # DW_FORM_strp
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	3                               # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	4                               # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	5                               # Abbreviation Code
	.byte	11                              # DW_TAG_lexical_block
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	6                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	23                              # DW_FORM_sec_offset
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	7                               # Abbreviation Code
	.byte	36                              # DW_TAG_base_type
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	62                              # DW_AT_encoding
	.byte	11                              # DW_FORM_data1
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	8                               # Abbreviation Code
	.byte	15                              # DW_TAG_pointer_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	9                               # Abbreviation Code
	.byte	38                              # DW_TAG_const_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	0                               # EOM(3)
	.section	.debug_info,"",@progbits
.Lcu_begin0:
	.long	.Ldebug_info_end0-.Ldebug_info_start0 # Length of Unit
.Ldebug_info_start0:
	.short	4                               # DWARF version number
	.long	.debug_abbrev                   # Offset Into Abbrev. Section
	.byte	8                               # Address Size (in bytes)
	.byte	1                               # Abbrev [1] 0xb:0xac DW_TAG_compile_unit
	.long	.Linfo_string0                  # DW_AT_producer
	.short	4                               # DW_AT_language
	.long	.Linfo_string1                  # DW_AT_name
	.long	.Lline_table_start0             # DW_AT_stmt_list
	.long	.Linfo_string2                  # DW_AT_comp_dir
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	2                               # Abbrev [2] 0x2a:0x6f DW_TAG_subprogram
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
                                        # DW_AT_GNU_all_call_sites
	.long	.Linfo_string3                  # DW_AT_linkage_name
	.long	.Linfo_string4                  # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
                                        # DW_AT_external
	.byte	3                               # Abbrev [3] 0x43:0xf DW_TAG_formal_parameter
	.long	.Ldebug_loc0                    # DW_AT_location
	.long	.Linfo_string5                  # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.long	153                             # DW_AT_type
	.byte	3                               # Abbrev [3] 0x52:0xf DW_TAG_formal_parameter
	.long	.Ldebug_loc1                    # DW_AT_location
	.long	.Linfo_string7                  # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.long	153                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x61:0xd DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	81
	.long	.Linfo_string8                  # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.long	160                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x6e:0xd DW_TAG_formal_parameter
	.byte	1                               # DW_AT_location
	.byte	82
	.long	.Linfo_string10                 # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.long	177                             # DW_AT_type
	.byte	5                               # Abbrev [5] 0x7b:0x1d DW_TAG_lexical_block
	.quad	.Ltmp0                          # DW_AT_low_pc
	.long	.Ltmp22-.Ltmp0                  # DW_AT_high_pc
	.byte	6                               # Abbrev [6] 0x88:0xf DW_TAG_variable
	.long	.Ldebug_loc2                    # DW_AT_location
	.long	.Linfo_string11                 # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.long	153                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	7                               # Abbrev [7] 0x99:0x7 DW_TAG_base_type
	.long	.Linfo_string6                  # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	8                               # Abbrev [8] 0xa0:0x5 DW_TAG_pointer_type
	.long	165                             # DW_AT_type
	.byte	9                               # Abbrev [9] 0xa5:0x5 DW_TAG_const_type
	.long	170                             # DW_AT_type
	.byte	7                               # Abbrev [7] 0xaa:0x7 DW_TAG_base_type
	.long	.Linfo_string9                  # DW_AT_name
	.byte	4                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	8                               # Abbrev [8] 0xb1:0x5 DW_TAG_pointer_type
	.long	170                             # DW_AT_type
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	"clang version 13.0.0 (https://github.com/Icohedron/KernelFaRer 134f4b204b58e994002ff5e4785ac2e24eee70f5)" # string offset=0
.Linfo_string1:
	.asciz	"simple_stencil.cc"             # string offset=105
.Linfo_string2:
	.asciz	"/home/icohedron/workspace/KernelFaRer" # string offset=123
.Linfo_string3:
	.asciz	"_Z13basicSstenciliiPKfPf"      # string offset=161
.Linfo_string4:
	.asciz	"basicSstencil"                 # string offset=186
.Linfo_string5:
	.asciz	"imin"                          # string offset=200
.Linfo_string6:
	.asciz	"int"                           # string offset=205
.Linfo_string7:
	.asciz	"imax"                          # string offset=209
.Linfo_string8:
	.asciz	"A"                             # string offset=214
.Linfo_string9:
	.asciz	"float"                         # string offset=216
.Linfo_string10:
	.asciz	"B"                             # string offset=222
.Linfo_string11:
	.asciz	"i"                             # string offset=224
	.ident	"clang version 13.0.0 (https://github.com/Icohedron/KernelFaRer 134f4b204b58e994002ff5e4785ac2e24eee70f5)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.section	.debug_line,"",@progbits
.Lline_table_start0:
