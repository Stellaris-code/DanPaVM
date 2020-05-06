#include "vm.h"

#include <string.h>

static uint8_t keep_array[MAX_MEMORY_REGIONS];

void process_var(vm_state_t *vm, var_t var)
{
    if (VAR_TYPE(var) < PTR)
        return;

    unsigned object = VAR_OBJECT(var);

    // prevent cyclic references
    if (keep_array[object])
        return;

    // mark the memory region pointer by this variable as 'keep'
    keep_array[object] = 1;

    memory_region_t* region = &vm->mem_regions[object];
    // now search through the pointed memory region
    const unsigned size = region->size;
    for (unsigned i = 0; i < size; ++i)
        process_var(vm, region->base[i]);
}

int run_garbage_collection(vm_state_t *vm)
{
    // clear the keep array
    memset(keep_array, 0, MAX_MEMORY_REGIONS*sizeof(uint8_t));

    // search thru global variables
    for (unsigned i = 0; i < VAR_COUNT; ++i)
        process_var(vm, vm->glob_vars[i]);

    // search thru local variables
    for (int depth = 0; depth <= vm->call_depth; ++depth)
    {
        for (unsigned i = 0; i < 64; ++i)
            process_var(vm, vm->loc_var_pages[depth][i]);
    }

    // search thru the data stack
    for (unsigned i = 0; i <= vm->sp; ++i)
        process_var(vm, vm->data_stack[i]);

    int freed_regions = 0;
    int freed_memory_size = 0;

    // now, deallocate all the memory regions that aren't marked as 'keep'
    for (unsigned i = 0; i < MAX_MEMORY_REGIONS; ++i)
    {
        if (!keep_array[i] && vm->mem_regions[i].base != NULL)
        {
            //printf("freed region %d of size %d\n", i, vm->mem_regions[i].size);
            freed_memory_size += vm->mem_regions[i].size;
            free_memory_region(vm, i);
            ++freed_regions;
        }
        else if (keep_array[i])
        {
            //printf("region was kept: %d of size %d\n", i, vm->mem_regions[i].size);
        }
    }

    //printf("ran GC; %d regions were freed (%d slots)\n", freed_regions, freed_memory_size);

    return freed_regions;
}
