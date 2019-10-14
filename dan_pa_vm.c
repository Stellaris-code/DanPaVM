#include "dan_pa_vm.h"

#include <stdlib.h>     //Memory management
#include <stdio.h>      //Output
#include <string.h>     //memcpy

#include "dpvm_opcodes.h"

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

        case (_JMP_):
            {
                int16_t offset = 0;

                memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, sizeof(offset));
                vm->_priv_program_pointer += sizeof(offset) + offset - 1;
                printf("TEST Jmp : offset = %d\n", offset);
            }
            break;

        case (_CALL_):
            {
                int16_t offset = 0;
                memcpy(&offset, vm->_priv_program + vm->_priv_program_pointer + 1, sizeof(offset));

                vm->_priv_program_pointer += sizeof(offset);

                if (_dpvm_priv_push_call(vm))
                {
                    fprintf(stderr, "DPVM : CALL : Failed to push current IP to call stack\n");
                    run = 0;
                }

                vm->_priv_program_pointer += offset - 1;
                printf("TEST Call : offset = %d\n", offset);
            }
            break;

        case (_RET_):
            if (_dpvm_priv_ret(vm))
            {
                printf("DPVM : Code returned at 0x%X with empty call stack, stopping.\n", vm->_priv_program_pointer);
                run = 0;
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
