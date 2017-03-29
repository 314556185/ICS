#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute () {
    // cmp S2, S1 <=> S1 - S2
    // cmp SRC, DESt <=> DEST : SRC
	DATA_TYPE rval = -op_src->val;
	DATA_TYPE result = op_dest->val + rval;
    /*
    Log("dest: %0x", op_dest->val);
    Log("src: %0x", op_src->val);
    Log("result: %0x", result);
    */

    // cmp 0x80000000, 0
    // Wrong CF = 1, PF = 1, SF = 1
    // cmp 0, 1
    // Wrong OF = 1
    // cmp 0x80000000, 0x80000001

	// OF, SF, ZF, CF, and PF
    uint32_t step = DATA_BYTE * 8 - 1;
    unsigned sf = 0x1 & (op_src->val >> step);
	unsigned df = 0x1 & (op_dest->val >> step);
	unsigned rf = 0x1 & (result >> step);

    // Set OF
    // sub: 0 1 1
    //      1 0 0
    // Set CF
    // Cout ^ Sub
    // Cout -> 0
    // uint32_t(dest) < uint32_t(src)
	cpu.eflags.bits.OF = (!df && sf && rf) || (df && !sf && !rf);
	cpu.eflags.bits.SF = rf;
	cpu.eflags.bits.ZF = (result == 0);
	cpu.eflags.bits.CF = (uint32_t)(op_dest->val) < (uint32_t)(op_src->val);

	result = 0xff & result;
	unsigned count;
	for(count = 0; result; ++count)
		result &= (result - 1);
	cpu.eflags.bits.PF = !(count % 2);

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
