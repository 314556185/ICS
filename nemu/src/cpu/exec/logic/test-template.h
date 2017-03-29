#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;

	// OF = 0, CF = 0; SF, ZF, and PF
	cpu.eflags.bits.OF = 0;
	cpu.eflags.bits.CF = 0;
	cpu.eflags.bits.SF = 0x1 & (result >> 31);
	cpu.eflags.bits.ZF = (result == 0);

	int count;
	result = (result & 0xff);
	for(count = 0; result; count++)
		result &= (result - 1);
	cpu.eflags.bits.PF = !(count % 2);

	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
