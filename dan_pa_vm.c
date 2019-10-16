#include "dan_pa_vm.h"

#include <stdlib.h>     //Memory management
#include <stdio.h>      //Output
#include <string.h>     //memcpy
#include <assert.h>

#include "dpvm_op_pointers.h"

//Other defines
//#define DATA_STACK_SIZE
//#define CALL_STACK_SIZE

DanPaVM dpvm_new_VM(const void* code)
{
    DanPaVM dpvm;

    //Init the code pointer
    dpvm._priv_program = (const uint8_t*)code;
    dpvm._priv_program_pointer = 0;

    //Init both stacks
    dpvm._priv_call_stack = malloc(sizeof(size_t) * 0x200); // 512 calls
    dpvm._priv_call_stack_pointer = 0;
    dpvm._priv_data_stack = malloc(sizeof(uint32_t) * 0x8000); //128 kB
    dpvm._priv_data_stack_pointer = 0;

    //Variables
    dpvm._priv_local_vars_stack = malloc(sizeof(void*) * (0x200 + 1)); //One local variables area per call + 1 for the entry
    dpvm._priv_local_vars_stack_pointer = 0;
    dpvm._priv_global_vars = calloc(0xFF, 4);

    dpvm._priv_error_flag = 0;

    dpvm._priv_run = 0;

    return dpvm;
}

void dpvm_delete_VM(DanPaVM* vm)
{
    free(vm->_priv_call_stack);
    free(vm->_priv_data_stack);
    free(vm->_priv_global_vars);
}

void dpvm_run(DanPaVM* vm, uint8_t entry)
{
    assert(vm->_priv_local_vars_stack_pointer == 0);

    vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer] = calloc(0xFF, 4);
    vm->_priv_local_vars_stack_pointer++;

    vm->_priv_program_pointer = entry;

    vm->_priv_run = 1;

    while (vm->_priv_run)
    {
        uint8_t op_code = vm->_priv_program[vm->_priv_program_pointer];

        vm->_priv_error_flag = 0;

        if (op_code)    //To ignore NOP (0)
            (*_dpvm_priv_op_functions[op_code])(vm);

        if (vm->_priv_error_flag & 1)   //Unknown instruction
        {
            fprintf(stderr, "DPVM : OP 0x%X unimplemented!\n");
            fprintf(stderr, "       Program pointer is 0x%X\n", vm->_priv_program_pointer);
            fprintf(stderr, "       Stopping VM.\n");

            vm->_priv_run = 0;

            break;
        }

        vm->_priv_program_pointer++;
    }

    assert(vm->_priv_local_vars_stack_pointer == 1);

    vm->_priv_local_vars_stack_pointer--;
    free(vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer]);

    vm->_priv_run = 0;
}





int _dpvm_priv_push_call(DanPaVM* vm)
{
    assert(vm->_priv_call_stack_pointer == vm->_priv_local_vars_stack_pointer - 1);

    if (vm->_priv_call_stack_pointer >= 0x200) //If the IP won't fit
        return 1;

    vm->_priv_call_stack[vm->_priv_call_stack_pointer] = vm->_priv_program_pointer + 2;
    vm->_priv_call_stack_pointer++;

    vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer] = calloc(0xFF, 4);
    vm->_priv_local_vars_stack_pointer++;

    return 0;
}

int _dpvm_priv_ret(DanPaVM* vm)
{
    assert(vm->_priv_call_stack_pointer == vm->_priv_local_vars_stack_pointer - 1);

    if (vm->_priv_call_stack_pointer == 0)
        return 1;

    vm->_priv_call_stack_pointer--;
    vm->_priv_program_pointer = vm->_priv_call_stack[vm->_priv_call_stack_pointer];

    vm->_priv_local_vars_stack_pointer--;
    free(vm->_priv_local_vars_stack[vm->_priv_local_vars_stack_pointer]);

    return 0;
}

int _dpvm_priv_push(DanPaVM* vm, void* src)
{
    if (vm->_priv_data_stack_pointer >= 0x8000)
        return 1;

    vm->_priv_data_stack[vm->_priv_data_stack_pointer] = *((int32_t*)src);

    vm->_priv_data_stack_pointer++;

    return 0;
}

int _dpvm_priv_pop(DanPaVM* vm, void* dest)
{
    if (vm->_priv_data_stack_pointer == 0)
        return 1;

    vm->_priv_data_stack_pointer--;

    *((int32_t*)dest) = vm->_priv_data_stack[vm->_priv_data_stack_pointer];

    return 0;
}
