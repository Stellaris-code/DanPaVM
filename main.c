#include "dan_pa_vm.h"

int main()
{
    unsigned char program[] = { 0xFF, 0xFF, 0x04 };

    DanPaVM dpvm = dpvm_newVM(program);

    dpvm_run(&dpvm);

    dpvm_deleteVM(&dpvm);

    return 0;
}
