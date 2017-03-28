#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE rval = -op_src->val;
	DATA_TYPE result = op_dest->val + rval;
	OPERAND_W(op_dest, result);

	// OF, SF, ZF, CF, PF
	uint32_t step = 0x8 * DATA_BYTE - 1;
	unsigned sf = 0x1 & (op_src->val >> step);
	unsigned df = 0x1 & (op_dest->val >> step);
	unsigned rf = 0x1 & (result >>step);

	cpu.eflags.OF = (!df && sf && rf) || (df && !sf && !rf);
	cpu.eflags.SF = rf;
	cpu.eflags.ZF = (result == 0);
	cpu.eflags.CF = (uint32_t) (op_dest->val) < (uint32_t)(op_src->val);

	result = 0xff & result;
	unsigned count;
	for(count = 0;result;++count)
		result &= (result - 1);
	cpu.eflags.PF = !(count % 2);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
