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

    //Data stack
    //Next available stack space is at data_stack + data_stack_pointer
    int32_t* _priv_data_stack;
    size_t _priv_data_stack_pointer;

} DanPaVM;

//Public functions
DanPaVM dpvm_new_VM(const void* code);

void dpvm_delete_VM(DanPaVM* vm);

void dpvm_run(DanPaVM* vm);

//Private functions
int _dpvm_priv_push_call(DanPaVM* vm);
int _dpvm_priv_ret(DanPaVM* vm);

int _dpvm_priv_push(DanPaVM* vm, void* src);
int _dpvm_priv_pop(DanPaVM* vm, void* dest);
