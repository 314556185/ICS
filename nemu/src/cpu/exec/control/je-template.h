#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	// IF condition
	// THEN
	// EIP <- EIP + SignExtend (rel8/16/32);
	// IF Operation = 16
	// THEN EIP <- EIP AND 0000FFFFH;
	// FI;
	// FI;
	if(cpu.eflags.bits.ZF) {
		REG(R_ESP) -= DATA_BYTE;
		cpu.eip += op_src->val;
		print_asm_template1();
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
