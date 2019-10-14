#include "dan_pa_vm.h"

#include <stdlib.h>     //Memory management
#include <stdio.h>      //Output
#include <string.h>     //memcpy

#include "dpvm_opcodes.h"

//Other defines
//#define DATA_STACK_SIZE
//#define CALL_STACK_SIZE

DanPaVM dpvm_new_VM(const void* code, void* global_vars, pthread_mutex_t* global_vars_mutex)
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
    dpvm._priv_local_vars = calloc(0xFF, sizeof(int32_t));
    dpvm._priv_global_vars = global_vars;
    dpvm._priv_global_vars_mutex = global_vars_mutex;   //We want thread safety with global vars!

    return dpvm;
}

void dpvm_delete_VM(DanPaVM* vm)
{
    free(vm->_priv_call_stack);
    free(vm->_priv_data_stack);
}

void dpvm_run(DanPaVM* vm)
{
    int run = 1;

    while (run)
    {
        uint8_t op_code = vm->_priv_program[vm->_priv_program_pointer];

        switch (op_code)
        {
        case (_NOP_): break;    //Do nothing

        //case (_SYSCALL_):break;

        case (_BRT_):
            {
                int32_t condition = 0;
                if (_dpvm_priv_pop(vm, &condition))
                {
                    fprintf(stderr, "DPVM : BRT : Failed to pop condition value\n");
                    run = 0;
                }

                if (condition)
                {
                    int16_t offset = 0;

                    memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);
                    vm->_priv_program_pointer += sizeof(offset) + offset - 1;
                }
            }
            break;

        case (_BRF_):
            {
                int32_t condition = 0;
                if (_dpvm_priv_pop(vm, &condition))
                {
                    fprintf(stderr, "DPVM : BRT : Failed to pop condition value\n");
                    run = 0;
                }

                if (!condition)
                {
                    int16_t offset = 0;

                    memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);
                    vm->_priv_program_pointer += sizeof(offset) + offset - 1;
                }
            }
            break;

        case (_JMP_):
            {
                int16_t offset = 0;

                memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);
                vm->_priv_program_pointer += sizeof(offset) + offset - 1;
            }
            break;

        case (_CALL_):
            {
                int16_t offset = 0;
                memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, 2);

                vm->_priv_program_pointer += sizeof(offset);

                if (_dpvm_priv_push_call(vm))
                {
                    fprintf(stderr, "DPVM : CALL : Failed to push current IP to call stack\n");
                    run = 0;
                }

                vm->_priv_program_pointer += offset - 1;
            }
            break;

        case (_RET_):
            if (_dpvm_priv_ret(vm))
                run = 0;
            break;

        case (_LDLOC_):
            {
                uint8_t local_var = 0;
                memcpy(&local_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

                vm->_priv_program_pointer += sizeof(local_var);

                int32_t var = vm->_priv_local_vars[local_var];

                if (_dpvm_priv_push(vm, &var))
                {
                    fprintf(stderr, "DPVM : LDLOC : Failed to push loaded value to stack\n");
                    run = 0;
                }
            }
            break;

        case (_LDGLB_):
            {
                uint8_t global_var = 0;
                memcpy(&global_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

                vm->_priv_program_pointer += sizeof(global_var);

                pthread_mutex_lock(vm->_priv_global_vars_mutex);
                int32_t var = vm->_priv_global_vars[global_var];
                pthread_mutex_unlock(vm->_priv_global_vars_mutex);

                if (_dpvm_priv_push(vm, &var))
                {
                    fprintf(stderr, "DPVM : LDGLB : Failed to push loaded value to stack\n");
                    run = 0;
                }
            }
            break;

        case (_STLOC_):
            {
                uint8_t local_var = 0;
                memcpy(&local_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

                vm->_priv_program_pointer += sizeof(local_var);

                int32_t value = 0;
                if (_dpvm_priv_pop(vm, &value))
                {
                    fprintf(stderr, "DPVM : STLOC : Failed to pop loaded value from stack\n");
                    run = 0;
                }
                else
                    vm->_priv_local_vars[local_var] = value;
            }
            break;

        case (_STGLB_):
            {
                uint8_t global_var = 0;
                memcpy(&global_var, vm->_priv_program + vm->_priv_program_pointer + 1, 1);

                vm->_priv_program_pointer += sizeof(global_var);

                int32_t value = 0;
                if (_dpvm_priv_pop(vm, &value))
                {
                    fprintf(stderr, "DPVM : STGLB : Failed to pop loaded value from stack\n");
                    run = 0;
                }
                else
                {
                    pthread_mutex_lock(vm->_priv_global_vars_mutex);
                    vm->_priv_global_vars[global_var] = value;
                    pthread_mutex_lock(vm->_priv_global_vars_mutex);
                }
            }
            break;

        case (_DBG_):   //Give debug info
            printf("DPVM Debug :\n");
            printf("    IP : 0x%X\n", vm->_priv_program_pointer);
            break;

        default:
            fprintf(stderr, "DPVM : Unknown opcode 0x%X (it may be still unimplemented)\n", op_code);
            fprintf(stderr, "       Program pointer is 0x%X\n", vm->_priv_program_pointer);
            fprintf(stderr, "       Stopping VM.\n");
            run = 0;
            break;
        }

        vm->_priv_program_pointer++;
    }
}





int _dpvm_priv_push_call(DanPaVM* vm)
{
    if (vm->_priv_call_stack_pointer >= 0x200) //If the IP won't fit
        return 1;

    vm->_priv_call_stack[vm->_priv_call_stack_pointer] = vm->_priv_program_pointer;

    vm->_priv_call_stack_pointer++;

    return 0;
}

int _dpvm_priv_ret(DanPaVM* vm)
{
    if (vm->_priv_call_stack_pointer == 0)
        return 1;

    vm->_priv_call_stack_pointer--;

    vm->_priv_program_pointer = vm->_priv_call_stack[vm->_priv_call_stack_pointer];

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
