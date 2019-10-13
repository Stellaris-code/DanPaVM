#include "dan_pa_vm.h"

#include <stdlib.h>     //Memory management
#include <stdio.h>      //Output
#include <string.h>     //memcpy

//OP codes
#define _NOP_       0x00
#define _SYSCALL_   0x01
#define _BRT_       0x02
#define _BRF_       0x03
#define _JMP_       0x04
#define _CALL_      0x05
#define _RET_       0x06

#define _DBG_       0xFF

DanPaVM dpvm_new_VM(const void* code)
{
    DanPaVM dpvm;

    //Init the code pointer
    dpvm._priv_program = (const uint8_t*)code;
    dpvm._priv_program_pointer = 0;

    //Init both stacks
    dpvm._priv_call_stack = malloc(sizeof(size_t) * 512); // 512 calls
    dpvm._priv_call_stack_pointer = 0;
    dpvm._priv_data_stack = malloc(0x20000); //128 kB
    dpvm._priv_data_stack_pointer = 0;

    //For testing, we put offset for our test call and jmp
    int16_t offset = 3;
    _dpvm_priv_push(&dpvm, &offset, sizeof(offset));
    _dpvm_priv_push(&dpvm, &offset, sizeof(offset));

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
                int16_t offset;
                if (_dpvm_priv_pop(vm, &offset, sizeof(offset)))
                {
                    fprintf(stderr, "DPVM : JMP : Failed to pop offset from data stack\n");
                    run = 0;
                }
                vm->_priv_program_pointer += (offset - 1);
            }
            break;

        case (_CALL_):
            {
                if (_dpvm_priv_push_call(vm))
                {
                    fprintf(stderr, "DPVM : CALL : Failed to push current IP to call stack\n");
                    run = 0;
                }

                int16_t offset;
                if (_dpvm_priv_pop(vm, &offset, sizeof(offset)))
                {
                    fprintf(stderr, "DPVM : CALL : Failed to pop offset from data stack\n");
                    run = 0;
                }
                vm->_priv_program_pointer += (offset - 1);
            }
            break;

        case (_RET_):
            if (_dpvm_priv_ret(vm))
            {
                fprintf(stderr, "DPVM : RET : Failed to pop IP from call stack\n");
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
    if (vm->_priv_call_stack_pointer >= 512) //If the IP won't fit
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

int _dpvm_priv_push(DanPaVM* vm, void* src, size_t data_size)
{
    if (vm->_priv_data_stack_pointer > 0x20000 - data_size)
        return 1;

    memcpy(vm->_priv_data_stack + vm->_priv_data_stack_pointer, src, data_size);

    vm->_priv_data_stack_pointer += data_size;

    return 0;
}

int _dpvm_priv_pop(DanPaVM* vm, void* dest, size_t data_size)
{
    if (vm->_priv_data_stack_pointer < data_size)
        return 1;

    vm->_priv_data_stack_pointer -= data_size;

    memcpy(dest, vm->_priv_data_stack + vm->_priv_data_stack_pointer, data_size);

    return 0;
}
