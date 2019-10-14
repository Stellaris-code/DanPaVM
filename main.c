#include "dan_pa_vm.h"

#include "dpvm_opcodes.h"

#include <stdlib.h>

#include <pthread.h>

int main()
{
    unsigned char program[] = { _DBG_, _CALL_, 4, 0,  _DBG_, _JMP_, 3, 0, _DBG_, _RET_, _DBG_, _RET_ };

    void* global = calloc(256, 4);
    pthread_mutex_t lock;

    pthread_mutex_init(&lock, NULL);

    DanPaVM dpvm = dpvm_new_VM(program, global, &lock);

    dpvm_run(&dpvm);

    dpvm_delete_VM(&dpvm);

    return 0;
}
