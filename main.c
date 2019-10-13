#include "dan_pa_vm.h"

int main()
{
    unsigned char program[] = { 0xFF, 0x05, 0xFF, 0x04, 0xFF, 0x06, 0xFF, 0x88 };

    DanPaVM dpvm = dpvm_new_VM(program);

    dpvm_run(&dpvm);

    dpvm_delete_VM(&dpvm);

    return 0;
}
