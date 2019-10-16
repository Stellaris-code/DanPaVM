#ifndef DPVM_OP_POINTERS_H
#define DPVM_OP_POINTERS_H

#include "dpvm_op_functions.h"

void (*_dpvm_priv_op_functions[0x100]) (DanPaVM* vm) =
{
    NULL,
    //Including this because having a file with each function pointer at the line number corresponding to the opcode is practical
    #include "dpvm_op_functions_list.h"
};

#endif // DPVM_OP_POINTERS_H
