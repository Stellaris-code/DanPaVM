#include "syscalls.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>


// placeholder
static void error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);

    exit(1);
}

static char* vmstr_to_cstr(vm_state_t* vm, var_t var)
{
    char* buf = malloc(vm->mem_regions[VAR_OBJECT(var)].size+1);
    char* ptr = buf;
    for (unsigned j = 0; j < vm->mem_regions[VAR_OBJECT(var)].size; ++j)
    {
        *ptr++ = VAR_VAL(vm->mem_regions[VAR_OBJECT(var)].base[j]);
    }
    *ptr++ = '\0';

    return buf;
}

static void syscall_printf(vm_state_t* vm)
{
    var_t arg_cnt_var = pop_stack(vm);
    assert(VAR_TYPE(arg_cnt_var) == INT);
    unsigned arg_cnt = VAR_VAL(arg_cnt_var);
    --arg_cnt; // ignore the fmt arg


    var_t fmt_args[arg_cnt];
    while (arg_cnt--)
    {
        fmt_args[arg_cnt] = pop_stack(vm);
    }
    var_t fmt = pop_stack(vm);
    int arg_idx = 0;

    unsigned fmt_len = vm->mem_regions[VAR_OBJECT(fmt)].size;
    for (unsigned i = 0; i < fmt_len; ++i)
    {
        char c = VAR_VAL(vm->mem_regions[VAR_OBJECT(fmt)].base[i]);
        if (c != '%' || i == fmt_len-1)
            printf("%c", c);
        else
        {
            ++i;
            char type = VAR_VAL(vm->mem_regions[VAR_OBJECT(fmt)].base[i]);
            if (type == 'd')
            {
                printf("%d", VAR_VAL(fmt_args[arg_idx]));
                ++arg_idx;
            }
            else if (type == 'f')
            {
                printf("%f", VAR_VAL_FLT(fmt_args[arg_idx]));
                ++arg_idx;
            }
            else if (type == 's')
            {
                var_t var = fmt_args[arg_idx];
                assert(VAR_TYPE(var) == STR);
                for (unsigned j = 0; j < vm->mem_regions[VAR_OBJECT(var)].size; ++j)
                {
                    putchar(VAR_VAL(vm->mem_regions[VAR_OBJECT(var)].base[j]));
                }
                ++arg_idx;
            }
        }
    }
}

static void read_int(vm_state_t* vm)
{
    int val;
    scanf("%d", &val);

    push_stack(vm, MK_VAR(val, INT));
}

static void read_str(vm_state_t* vm)
{
    char buf[256];
    scanf("%s", buf);

    unsigned buf_len = strlen(buf);

    unsigned obj = alloc_memory_region(vm, buf_len);
    var_t var = MK_STR(obj);

    for (unsigned i = 0; i < buf_len; ++i)
    {
        vm->mem_regions[obj].base[i] = MK_VAR(buf[i], INT);
    }

    push_stack(vm, var);
}

static void vm_exit(vm_state_t* vm)
{
    vm->stopped = 1;
}

static clock_t clock_start;
static void start_clock(vm_state_t* vm)
{
    (void)vm;
    clock_start = clock();
}

static void stop_clock(vm_state_t* vm)
{
    (void)vm;
    clock_t stop = clock();
    unsigned int milliseconds = (stop - clock_start)*1000/CLOCKS_PER_SEC;

    printf("milliseconds : %d\n", milliseconds);
}

static void syscall_assert(vm_state_t* vm)
{
    int line = VAR_VAL(pop_stack(vm));
    var_t filename_var = pop_stack(vm);
    var_t line_str_var = pop_stack(vm);
    var_t cond_var = pop_stack(vm);
    int result = BOOL_TEST(cond_var);
    if (!result)
    {
        char* filename = vmstr_to_cstr(vm, filename_var);
        char* line_str = vmstr_to_cstr(vm, line_str_var);
        fprintf(stderr, "Assertion failed!\n\nFile %s:%d\nExpression : %s\n", filename, line, line_str);
        free(filename);
        free(line_str);
        abort();
    }
}

static void invalid_syscall(vm_state_t* vm)
{
    (void)vm;
    printf("bad syscall\n");
    abort();
}
typedef void(*syscall_callback)(vm_state_t*);

syscall_callback base_syscalls[256] =
{
    [0 ... 255] = invalid_syscall,

    [0] = syscall_printf,
    [1] = read_int,
    [2] = read_str,
    [3] = vm_exit,
    [0x10] = start_clock,
    [0x11] = stop_clock,
    [0x20] = syscall_assert
};

void vm_syscall(vm_state_t* vm, int syscall_id)
{
    // base syscall
    if (syscall_id < 256)
    {
        syscall_callback fn = base_syscalls[syscall_id];
        fn(vm);
    }
    else
    {
        // no extension syscalls yet
        invalid_syscall(vm);
    }
}
