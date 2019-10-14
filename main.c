#include "dan_pa_vm.h"

#include "dpvm_opcodes.h"

int main()
{
    unsigned char program[] = { _DBG_, _CALL_, 4, 0,  _DBG_, _JMP_, 3, 0, _DBG_, _RET_, _DBG_, _RET_ };

    DanPaVM dpvm = dpvm_new_VM(program);

    dpvm_run(&dpvm);

    dpvm_delete_VM(&dpvm);

    return 0;
}
