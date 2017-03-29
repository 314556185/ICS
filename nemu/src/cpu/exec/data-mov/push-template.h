#include "cpu/exec/template-start.h"

#define instr push

static void do_execute () {
    // IF OperandsSize = 16
    // THEN
    //  ESP <- ESP - 2;
    //  (SS:ESP) <- (SOURCE); (* word assignment *)
    // ELSE
	//  ESP = ESP - 4;
	//  (SS:ESP) <- (SOURCE); (* dword assignment *)
	REG(R_ESP) -= 4;
	MEM_W(REG(R_ESP),op_src->val);
	print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
