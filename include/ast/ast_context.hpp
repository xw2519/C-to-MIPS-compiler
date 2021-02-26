#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <vector>
#include <map>
#include <cassert>

/*
$0	        $zero	    Hard-wired to 0
$1	        $at	        Reserved for pseudo-instructions
$2  - $3	$v0,  $v1	Return values from functions
$4  - $7	$a0 - $a3	Arguments to functions - not preserved by subprograms
$8  - $15	$t0 - $t7	Temporary data, not preserved by subprograms
$16 - $23	$s0 - $s7	Saved registers, preserved by subprograms
$24 - $25	$t8 - $t9	More temporary registers, not preserved by subprograms
$26 - $27	$k0 - $k1	Reserved for kernel. Do not use.
$28	        $gp	        Global Area Pointer (base of global data segment)
$29	        $sp	        Stack Pointer
$30	        $fp	        Frame Pointer
$31	        $ra	        Return Address
*/

struct Ast_Context
{

};


#endif