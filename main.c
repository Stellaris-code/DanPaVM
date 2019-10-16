#include "dan_pa_vm.h"

#include "dpvm_opcodes.h"

#include <stdlib.h>

int main()
{
    unsigned char program[] =
    {
        _DBG_,
        _PUSH_, 3, _STLOC_, 3, _DBG_,
        _PUSH_, 6, _STGLB_, 0, _DBG_,
        _CALL_, 5, 0,
        _DBG_, _RET_,
        _PUSH_, 3, _STLOC_, 0, _DBG_,
        _RET_
    };


    DanPaVM dpvm = dpvm_new_VM(program);

    dpvm_run(&dpvm, 0);

    dpvm_delete_VM(&dpvm);

    return 0;
}
