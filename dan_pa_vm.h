#ifndef DAN_PA_VM_H
#define DAN_PA_VM_H

#include <stddef.h>     //size_t
#include <stdint.h>     //Fixed size ints

typedef struct
{
    //Program pointer
    //Real program pointer is at code_pointer + program_pointer
    const uint8_t* _priv_program;
    uint32_t _priv_program_pointer;

    //Call stack
    //Next available stack space is at call_stack + call_stack_pointer
    uint32_t* _priv_call_stack;
    size_t _priv_call_stack_pointer;
    //For each call theres an array of 256 values of 32 bits allocated, for variables local to the function
    int32_t** _priv_local_vars_stack;
    size_t _priv_local_vars_stack_pointer;

    //Data stack
    //Next available stack space is at data_stack + data_stack_pointer
    int32_t* _priv_data_stack;
    size_t _priv_data_stack_pointer;

    //Variables
    int32_t* _priv_global_vars;

    //Misc
    uint32_t _priv_error_flag;
    int _priv_run;
} DanPaVM;

//Public functions
DanPaVM dpvm_new_VM(const void* code);

void dpvm_delete_VM(DanPaVM* vm);

void dpvm_run(DanPaVM* vm, uint8_t entry);

//Private functions
int _dpvm_priv_push_call(DanPaVM* vm);
int _dpvm_priv_ret(DanPaVM* vm);

int _dpvm_priv_push(DanPaVM* vm, void* src);
int _dpvm_priv_pop(DanPaVM* vm, void* dest);

#endif // DAN_PA_VM_H
