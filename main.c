#include "dan_pa_vm.h"

#include <stdlib.h>

#include "dpvm_opcodes.h"

int main()
{
    unsigned char program[] =
    {
        _DBG_,
        _RET_
    };

    DanPaVM dpvm = dpvm_new_VM(program);

    dpvm_run(&dpvm, 0);

    dpvm_delete_VM(&dpvm);


    return 0;
}
