#include <stddef.h>     //size_t
#include <stdint.h>     //Fixed size ints

typedef struct
{
    //Program pointer
    //Real program pointer is at code_pointer + program_pointer
    const uint8_t* _priv_program;
    size_t _priv_program_pointer;

    //Call stack
    //Next available stack space is at call_stack + call_stack_pointer
    size_t* _priv_call_stack;
    size_t _priv_call_stack_pointer;

    //Data stack
    //Next available stack space is at data_stack + data_stack_pointer
    void* _priv_data_stack;
    size_t _priv_data_stack_pointer;

} DanPAVM;

//Public functions
DanPAVM dpvm_newVM(const void* code);

void dpvm_deleteVM(DanPAVM* vm);

void dpvm_run(DanPAVM* vm);

//Private functions
int _dpvm_priv_push_call(DanPAVM* vm);
int _dpvm_priv_ret(DanPAVM* vm);

int _dpvm_priv_push(DanPAVM* vm, void* src, size_t data_size);
int _dpvm_priv_pop(DanPAVM* vm, void* dest, size_t data_size);
