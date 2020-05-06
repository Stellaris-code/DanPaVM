#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vm.h"

// TODO: ditch the use of get_prim_expr_type everywhere and replace it with a cached type_t in primary_expression_t

int main()
{

    FILE *input = fopen("in.bin", "rb");
    fseek(input, 0, SEEK_END);
    long fsize = ftell(input);
    rewind(input);  /* same as rewind(f); */

    if (fsize <= 0)
    {
        fprintf(stderr, "could not read input file");
        return -1;
    }

    printf("size of the vm_state structure : %llu\n", sizeof(vm_state_t));

    char signature[4];
    fread(signature, 1, 4, input);

    if (strncmp(signature, "DNPX", 4) != 0)
    {
        printf("invalid signature\n");
        abort();
    }

    uint32_t init_addr;
    fread(&init_addr, sizeof(uint32_t), 1, input);

    uint16_t string_table_size;
    fread(&string_table_size, sizeof(uint16_t), 1, input);
    string_entry_t* string_table = malloc(sizeof(string_entry_t) * string_table_size);
    for (int i = 0; i < string_table_size; ++i)
    {
        fread(&string_table[i].str_len, sizeof(uint16_t), 1, input);
        string_table[i].str = malloc(string_table[i].str_len);
        fread(string_table[i].str, 1, string_table[i].str_len, input);
    }

    uint8_t* buffer = malloc(fsize);
    fread(buffer, 1, fsize, input);
    fclose(input);

    vm_state_t* vm = calloc(1, sizeof(vm_state_t));
    vm->exec_buffer = buffer;
    vm->pc = init_addr;
    vm->rand_seed = time(0);
    vm->string_table = string_table;
    vm->string_table_size = string_table_size;
    vm->sp = 0xFFF8;

    run(vm);

    run_garbage_collection(vm);

    for (int i = 0; i < string_table_size; ++i)
    {
        free(string_table[i].str);
    }
    free(string_table);
    free(vm);
    free(buffer);
    return 0;
}
