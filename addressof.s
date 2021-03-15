	.file	1 "addressof.c"
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
 # compiler_tests/pointer/addressof.c -meb -march=mips32r2 -mfpxx -mllsc
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
	.frame	$fp,48,$31		# vars= 16, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48	 #,,
	sw	$31,44($sp)	 #,
	sw	$fp,40($sp)	 #,
	move	$fp,$sp	 #,
	lui	$28,%hi(__gnu_local_gp)	 #,
	addiu	$28,$28,%lo(__gnu_local_gp)	 #,,
	.cprestore	16	 #
 # compiler_tests/pointer/addressof.c:2: {
	lw	$2,%got(__stack_chk_guard)($28)	 # tmp196,,
	lw	$2,0($2)		 # tmp197, __stack_chk_guard
	sw	$2,36($fp)	 # tmp197, D.1441
 # compiler_tests/pointer/addressof.c:4:     int *y=&x;
	addiu	$2,$fp,28	 # tmp198,,
	sw	$2,32($fp)	 # tmp198, y
 # compiler_tests/pointer/addressof.c:5:     return 13;
	li	$2,13			# 0xd	 # _2,
	move	$4,$2	 # <retval>, _2
 # compiler_tests/pointer/addressof.c:6: }
	lw	$2,%got(__stack_chk_guard)($28)	 # tmp200,,
	lw	$3,36($fp)		 # tmp201, D.1441
	lw	$2,0($2)		 # tmp202, __stack_chk_guard
	beq	$3,$2,$L3
	nop
	 #, tmp201, tmp202,
	lw	$2,%call16(__stack_chk_fail)($28)	 # tmp203,,
	move	$25,$2	 # tmp203, tmp203
	.reloc	1f,R_MIPS_JALR,__stack_chk_fail	 #
1:	jalr	$25
	nop
	 # tmp203
$L3:
	move	$2,$4	 #, <retval>
	move	$sp,$fp	 #,
	lw	$31,44($sp)		 #,
	lw	$fp,40($sp)		 #,
	addiu	$sp,$sp,48	 #,,
	jr	$31
	nop
	 #
	.set	macro
	.set	reorder
	.end	f
	.size	f, .-f
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
