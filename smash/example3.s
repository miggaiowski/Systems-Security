	.section	__TEXT,__text,regular,pure_instructions
	.globl	_function
	.align	4, 0x90
_function:
Leh_func_begin1:
	pushq	%rbp
Ltmp0:
	movq	%rsp, %rbp
Ltmp1:
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movb	$97, -18(%rbp)
	leaq	-17(%rbp), %rax
	movb	$98, -17(%rbp)
	movb	$99, -13(%rbp)
	movabsq	$12, %rcx
	addq	%rcx, %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	addl	$8, %eax
	movq	-40(%rbp), %rcx
	movl	%eax, (%rcx)
	popq	%rbp
	ret
Leh_func_end1:

	.globl	_main
	.align	4, 0x90
_main:
Leh_func_begin2:
	pushq	%rbp
Ltmp2:
	movq	%rsp, %rbp
Ltmp3:
	subq	$16, %rsp
Ltmp4:
	movl	$0, -8(%rbp)
	movl	$1, %eax
	movl	$2, %ecx
	movl	$3, %edx
	movl	%eax, %edi
	movl	%ecx, %esi
	callq	_function
	movl	$1, -8(%rbp)
	movl	-8(%rbp), %ecx
	xorb	%dl, %dl
	leaq	L_.str(%rip), %rdi
	movl	%ecx, %esi
	movb	%dl, %al
	callq	_printf
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	ret
Leh_func_end2:

	.section	__TEXT,__cstring,cstring_literals
L_.str:
	.asciz	 "%d\n"

	.section	__TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame0:
Lsection_eh_frame:
Leh_frame_common:
Lset0 = Leh_frame_common_end-Leh_frame_common_begin
	.long	Lset0
Leh_frame_common_begin:
	.long	0
	.byte	1
	.asciz	 "zR"
	.byte	1
	.byte	120
	.byte	16
	.byte	1
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	3
Leh_frame_common_end:
	.globl	_function.eh
_function.eh:
Lset1 = Leh_frame_end1-Leh_frame_begin1
	.long	Lset1
Leh_frame_begin1:
Lset2 = Leh_frame_begin1-Leh_frame_common
	.long	Lset2
Ltmp5:
	.quad	Leh_func_begin1-Ltmp5
Lset3 = Leh_func_end1-Leh_func_begin1
	.quad	Lset3
	.byte	0
	.byte	4
Lset4 = Ltmp0-Leh_func_begin1
	.long	Lset4
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset5 = Ltmp1-Ltmp0
	.long	Lset5
	.byte	13
	.byte	6
	.align	3
Leh_frame_end1:

	.globl	_main.eh
_main.eh:
Lset6 = Leh_frame_end2-Leh_frame_begin2
	.long	Lset6
Leh_frame_begin2:
Lset7 = Leh_frame_begin2-Leh_frame_common
	.long	Lset7
Ltmp6:
	.quad	Leh_func_begin2-Ltmp6
Lset8 = Leh_func_end2-Leh_func_begin2
	.quad	Lset8
	.byte	0
	.byte	4
Lset9 = Ltmp2-Leh_func_begin2
	.long	Lset9
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset10 = Ltmp3-Ltmp2
	.long	Lset10
	.byte	13
	.byte	6
	.align	3
Leh_frame_end2:


.subsections_via_symbols
