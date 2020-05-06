#include "vm.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

uint16_t alloc_memory_region(vm_state_t *vm, uint32_t size)
{
    for (uint32_t i = vm->next_likely_free_region; i < MAX_MEMORY_REGIONS; ++i)
    {
        // free
        if (vm->mem_regions[i].base == NULL)
        {
            vm->mem_regions[i].base = calloc(size, sizeof(var_t));
            //vm->mem_regions[i].base = malloc(size * sizeof(var_t));
            vm->mem_regions[i].size = size;

            vm->next_likely_free_region = i+1;
            return i;
        }
    }
    if (vm->next_likely_free_region != 0)
    {
        vm->next_likely_free_region = 0;
// two different strategies :
// try searching again
#if 0
        // try searching again, starting from the beginning of the memory this time
        return alloc_memory_region(vm, size);
#else // reset next_likely_region pointer and run gc immediately after
        ;
#endif
    }

    // if we are out of memory regions, try running the gc
    if (run_garbage_collection(vm))
    {
        // tail-call to retry
        return alloc_memory_region(vm, size);
    }

    // if we reach this point, even running the GC couldn't help

    fprintf(stderr, "out of memory regions\n");
    abort();
}

void resize_memory_region(vm_state_t *vm, uint16_t region_id, uint32_t size)
{
    vm->mem_regions[region_id].base = realloc(vm->mem_regions[region_id].base, size * sizeof(var_t));
    vm->mem_regions[region_id].size = size;
}

void free_memory_region(vm_state_t *vm, uint16_t region_id)
{
    assert(vm->mem_regions[region_id].base != NULL);

    free(vm->mem_regions[region_id].base);
    vm->mem_regions[region_id].base = NULL;
    vm->mem_regions[region_id].size = 0;
}
