#include "dpvm_op_functions.h"

#include <stdlib.h>     //Memory management
#include <stdio.h>      //Output
#include <string.h>     //memcpy
#include <assert.h>

void _dpvm_priv_unknown_op(DanPaVM* vm)
{
    vm->_priv_error_flag |= 1;  //Unknown opcode flag
}

void _dpvm_priv_SYSCALL_op(DanPaVM* vm)
{
    fprintf(stderr, "DPVM : SYSCALL unimplemented, stopping.\n");
    vm->_priv_run = 0;
}
void _dpvm_priv_BRT_op(DanPaVM* vm)
{
    int32_t condition = 0;
    if (_dpvm_priv_pop(vm, &condition))
    {
        fprintf(stderr, "DPVM : BRT : Failed to pop condition value\n");
        vm->_priv_run = 0;
    }

    if (condition)
    {
        int16_t offset = 0;

        memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);

        vm->_priv_program_pointer += offset - 1;
    }
}
void _dpvm_priv_BRF_op(DanPaVM* vm)
{
    int32_t condition = 0;
    if (_dpvm_priv_pop(vm, &condition))
    {
        fprintf(stderr, "DPVM : BRT : Failed to pop condition value\n");
        vm->_priv_run = 0;
    }

    if (!condition)
    {
        int16_t offset = 0;

        memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);
        vm->_priv_program_pointer += offset - 1;
    }
}
void _dpvm_priv_JMP_op(DanPaVM* vm)
{
    int16_t offset = 0;

    memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);
    vm->_priv_program_pointer += offset - 1;
}
void _dpvm_priv_CALL_op(DanPaVM* vm)
{
    int16_t offset = 0;
    memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);

    if (_dpvm_priv_push_call(vm))
    {
        fprintf(stderr, "DPVM : CALL : Failed to push current IP to call stack\n");
        vm->_priv_run = 0;
    }

    vm->_priv_program_pointer += offset - 1;
}
void _dpvm_priv_RET_op(DanPaVM* vm)
{
    if (_dpvm_priv_ret(vm))
        vm->_priv_run = 0;
}

void _dpvm_priv_LDLOC_op(DanPaVM* vm)
{
    uint8_t local_var = 0;
    memcpy(&local_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

    vm->_priv_program_pointer += sizeof(local_var);

    int32_t var = vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer - 1][local_var];

    if (_dpvm_priv_push(vm, &var))
    {
        fprintf(stderr, "DPVM : LDLOC : Failed to push loaded value to stack\n");
        vm->_priv_run = 0;
    }
}
void _dpvm_priv_LDGLB_op(DanPaVM* vm)
{
    uint8_t global_var = 0;
    memcpy(&global_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

    vm->_priv_program_pointer += sizeof(global_var);

    int32_t var = vm->_priv_global_vars[global_var];

    if (_dpvm_priv_push(vm, &var))
    {
        fprintf(stderr, "DPVM : LDGLB : Failed to push loaded value to stack\n");
        vm->_priv_run = 0;
    }
}
void _dpvm_priv_STLOC_op(DanPaVM* vm)
{
    uint8_t local_var = 0;
    memcpy(&local_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

    vm->_priv_program_pointer += sizeof(local_var);

    int32_t value = 0;
    if (_dpvm_priv_pop(vm, &value))
    {
        fprintf(stderr, "DPVM : STLOC : Failed to pop loaded value from stack\n");
        vm->_priv_run = 0;
    }
    else
        vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer - 1][local_var] = value;
}
void _dpvm_priv_STGLB_op(DanPaVM* vm)
{
    uint8_t global_var = 0;
    memcpy(&global_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

    vm->_priv_program_pointer += sizeof(global_var);

    int32_t value = 0;
    if (_dpvm_priv_pop(vm, &value))
    {
        fprintf(stderr, "DPVM : STGLB : Failed to pop loaded value from stack\n");
        vm->_priv_run = 0;
    }
    else
        vm->_priv_global_vars[global_var] = value;
}

void _dpvm_priv_ARLD_op(DanPaVM* vm)
{
    fprintf(stderr, "DPVM : ARLD unimplemented, stopping.\n");
    vm->_priv_run = 0;
}
void _dpvm_priv_ARST_op(DanPaVM* vm)
{
    fprintf(stderr, "DPVM : ARST unimplemented, stopping.\n");
    vm->_priv_run = 0;
}
void _dpvm_priv_ARLEN_op(DanPaVM* vm)
{
    fprintf(stderr, "DPVM : ARLEN unimplemented, stopping.\n");
    vm->_priv_run = 0;
}
void _dpvm_priv_ARRESIZE_op(DanPaVM* vm)
{
    fprintf(stderr, "DPVM : ARRESIZE unimplemented, stopping.\n");
    vm->_priv_run = 0;
}

void _dpvm_priv_ADDI_op(DanPaVM* vm)
{}
void _dpvm_priv_SUBI_op(DanPaVM* vm)
{}
void _dpvm_priv_AND_op(DanPaVM* vm)
{}
void _dpvm_priv_OR_op(DanPaVM* vm)
{}
void _dpvm_priv_XOR_op(DanPaVM* vm)
{}
void _dpvm_priv_NOT_op(DanPaVM* vm)
{}
void _dpvm_priv_SHL_op(DanPaVM* vm)
{}
void _dpvm_priv_SHR_op(DanPaVM* vm)
{}
void _dpvm_priv_LSR_op(DanPaVM* vm)
{}
void _dpvm_priv_NEGI_op(DanPaVM* vm)
{}
void _dpvm_priv_MULI_op(DanPaVM* vm)
{}
void _dpvm_priv_DIVI_op(DanPaVM* vm)
{}
void _dpvm_priv_MODI_op(DanPaVM* vm)
{}
void _dpvm_priv_INC_op(DanPaVM* vm)
{}
void _dpvm_priv_DEC_op(DanPaVM* vm)
{}

void _dpvm_priv_ADDF_op(DanPaVM* vm)
{}
void _dpvm_priv_SUBF_op(DanPaVM* vm)
{}
void _dpvm_priv_NEGF_op(DanPaVM* vm)
{}
void _dpvm_priv_MULF_op(DanPaVM* vm)
{}
void _dpvm_priv_DIVF_op(DanPaVM* vm)
{}
void _dpvm_priv_MODF_op(DanPaVM* vm)
{}

void _dpvm_priv_PUSH_op(DanPaVM* vm)
{}
void _dpvm_priv_SWAP_op(DanPaVM* vm)
{}
void _dpvm_priv_DUP_op(DanPaVM* vm)
{}

void _dpvm_priv_I2F_op(DanPaVM* vm)
{}
void _dpvm_priv_F2I_op(DanPaVM* vm)
{}
void _dpvm_priv_I2C_op(DanPaVM* vm)
{}
void _dpvm_priv_C2I_op(DanPaVM* vm)
{}

void _dpvm_priv_DBG_op(DanPaVM* vm)
{
    printf("DPVM Debug :\n");

}



