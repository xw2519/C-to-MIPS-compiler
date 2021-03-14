	.file	1 "five_arg.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=xx
	.module	nooddspreg
	.abicalls

 # -G value = 0, Arch = mips32r2, ISA = 33
 # GNU C17 (Ubuntu 9.3.0-17ubuntu1~20.04) version 9.3.0 (mips-linux-gnu)
 #	compiled by GNU C version 9.3.0, GMP version 6.2.0, MPFR version 4.0.2, MPC version 1.1.0, isl version isl-0.22.1-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed:  -imultiarch mips-linux-gnu
 # compiler_tests/local_var/five_arg.c -meb -march=mips32r2 -mfpxx -mllsc
 # -mno-lxc1-sxc1 -mips32r2 -mno-shared -mabi=32 -fverbose-asm
 # -fstack-protector-strong -Wformat -Wformat-security
 # options enabled:  -faggressive-loop-optimizations -fassume-phsa
 # -fauto-inc-dec -fcommon -fdelete-null-pointer-checks -fdwarf2-cfi-asm
 # -fearly-inlining -feliminate-unused-debug-types -ffp-int-builtin-inexact
 # -ffunction-cse -fgcse-lm -fgnu-runtime -fgnu-unique -fident
 # -finline-atomics -fipa-stack-alignment -fira-hoist-pressure
 # -fira-share-save-slots -fira-share-spill-slots -fivopts
 # -fkeep-static-consts -fleading-underscore -flifetime-dse
 # -flto-odr-type-merging -fmath-errno -fmerge-debug-strings
 # -fpcc-struct-return -fpeephole -fpic -fplt -fprefetch-loop-arrays
 # -fsched-critical-path-heuristic -fsched-dep-count-heuristic
 # -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
 # -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
 # -fsched-stalled-insns-dep -fschedule-fusion -fsemantic-interposition
 # -fshow-column -fshrink-wrap-separate -fsigned-zeros
 # -fsplit-ivs-in-unroller -fssa-backprop -fstack-protector-strong
 # -fstdarg-opt -fstrict-volatile-bitfields -fsync-libcalls -ftrapping-math
 # -ftree-cselim -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im
 # -ftree-loop-ivcanon -ftree-loop-optimize -ftree-parallelize-loops=
 # -ftree-phiprop -ftree-reassoc -ftree-scev-cprop -funit-at-a-time
 # -fverbose-asm -fzero-initialized-in-bss -mabicalls -mcheck-zero-division
 # -mdivide-traps -mdouble-float -meb -mexplicit-relocs -mextern-sdata
 # -mfp-exceptions -mfp32 -mfpxx -mglibc -mgp32 -mgpopt -mhard-float
 # -mimadd -mllsc -mload-store-pairs -mlocal-sdata -mlong32 -mlra -mmadd4
 # -mno-mdmx -mno-mips16 -mno-mips3d -mrelax-pic-calls -msplit-addresses

	.text
	.align	2
	.globl	f
	.set	nomips16
	.set	nomicromips
	.ent	f
	.type	f, @function
f:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8	 #,,
	sw	$fp,4($sp)	 #,
	move	$fp,$sp	 #,
	sw	$4,8($fp)	 # a, a
	sw	$5,12($fp)	 # b, b
	sw	$6,16($fp)	 # c, c
	sw	$7,20($fp)	 # d, d
 # compiler_tests/local_var/five_arg.c:4:     return a + b + c + d + e;
	lw	$3,8($fp)		 # tmp199, a
	lw	$2,12($fp)		 # tmp200, b
	addu	$3,$3,$2	 # _1, tmp199, tmp200
 # compiler_tests/local_var/five_arg.c:4:     return a + b + c + d + e;
	lw	$2,16($fp)		 # tmp201, c
	addu	$3,$3,$2	 # _2, _1, tmp201
 # compiler_tests/local_var/five_arg.c:4:     return a + b + c + d + e;
	lw	$2,20($fp)		 # tmp202, d
	addu	$3,$3,$2	 # _3, _2, tmp202
 # compiler_tests/local_var/five_arg.c:4:     return a + b + c + d + e;
	lw	$2,24($fp)		 # tmp203, e
	addu	$2,$3,$2	 # _9, _3, tmp203
 # compiler_tests/local_var/five_arg.c:5: }
	move	$sp,$fp	 #,
	lw	$fp,4($sp)		 #,
	addiu	$sp,$sp,8	 #,,
	jr	$31
	nop
	 #
	.set	macro
	.set	reorder
	.end	f
	.size	f, .-f
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
