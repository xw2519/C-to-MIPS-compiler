	.file	1 "five_arg_driver.c"
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
 # compiler_tests/local_var/five_arg_driver.c -meb -march=mips32r2 -mfpxx
 # -mllsc -mno-lxc1-sxc1 -mips32r2 -mno-shared -mabi=32 -fverbose-asm
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
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,40,$31		# vars= 0, regs= 2/0, args= 24, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40	 #,,
	sw	$31,36($sp)	 #,
	sw	$fp,32($sp)	 #,
	move	$fp,$sp	 #,
	lui	$28,%hi(__gnu_local_gp)	 #,
	addiu	$28,$28,%lo(__gnu_local_gp)	 #,,
	.cprestore	24	 #
 # compiler_tests/local_var/five_arg_driver.c:6:     return !(f(10, 20, 30, 40, 50)==150);
	li	$2,50			# 0x32	 # tmp198,
	sw	$2,16($sp)	 # tmp198,
	li	$7,40			# 0x28	 #,
	li	$6,30			# 0x1e	 #,
	li	$5,20			# 0x14	 #,
	li	$4,10			# 0xa	 #,
	lw	$2,%call16(f)($28)	 # tmp199,,
	move	$25,$2	 # tmp199, tmp199
	.reloc	1f,R_MIPS_JALR,f	 #
1:	jalr	$25
	nop
	 # tmp199
	lw	$28,24($fp)		 #,
 # compiler_tests/local_var/five_arg_driver.c:6:     return !(f(10, 20, 30, 40, 50)==150);
	xori	$2,$2,0x96	 # tmp202, _1,
	sltu	$2,$0,$2	 # tmp201, tmp202
	andi	$2,$2,0x00ff	 # _2, tmp200
 # compiler_tests/local_var/five_arg_driver.c:7: }
	move	$sp,$fp	 #,
	lw	$31,36($sp)		 #,
	lw	$fp,32($sp)		 #,
	addiu	$sp,$sp,40	 #,,
	jr	$31
	nop
	 #
	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
