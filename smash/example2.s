	.section	__TEXT,__text,regular,pure_instructions
	.globl	_function
	.align	4, 0x90
_function:
Leh_func_begin1:
	pushq	%rbp
Ltmp0:
	movq	%rsp, %rbp
Ltmp1:
	subq	$32, %rsp
Ltmp2:
	movq	%rdi, -8(%rbp)
	leaq	-32(%rbp), %rax
	movq	-8(%rbp), %rcx
	movabsq	$16, %rdx
	movq	%rax, %rdi
	movq	%rcx, %rsi
	callq	___strcpy_chk
	movq	%rax, -16(%rbp)
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end1:

	.globl	_main
	.align	4, 0x90
_main:
Leh_func_begin2:
	pushq	%rbp
Ltmp3:
	movq	%rsp, %rbp
Ltmp4:
	subq	$272, %rsp
Ltmp5:
	movl	$0, -260(%rbp)
	jmp	LBB2_2
LBB2_1:
	movl	-260(%rbp), %eax
	movslq	%eax, %rax
	movb	$65, -256(%rbp,%rax)
	movl	-260(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -260(%rbp)
LBB2_2:
	movl	-260(%rbp), %eax
	cmpl	$254, %eax
	jle	LBB2_1
	leaq	-256(%rbp), %rax
	movq	%rax, %rdi
	callq	_function
	addq	$272, %rsp
	popq	%rbp
	ret
Leh_func_end2:

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
Ltmp6:
	.quad	Leh_func_begin1-Ltmp6
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
Ltmp7:
	.quad	Leh_func_begin2-Ltmp7
Lset8 = Leh_func_end2-Leh_func_begin2
	.quad	Lset8
	.byte	0
	.byte	4
Lset9 = Ltmp3-Leh_func_begin2
	.long	Lset9
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset10 = Ltmp4-Ltmp3
	.long	Lset10
	.byte	13
	.byte	6
	.align	3
Leh_frame_end2:


.subsections_via_symbols
