#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>

#include "syscalls.h"

#define INTERP_IMPL

#ifndef USE_COMPUTED_GOTOS

#define CODEBUF vm->exec_buffer
#define DATASTACK vm->data_stack
#define PUSH_STACK(val) push_stack(vm, val)
#define POP_STACK()     pop_stack(vm)
#define STACK_TOP_VALUE() DATASTACK[vm->sp]
#define SET_STACK_TOP_VAL(val) VAR_SET_VAL(DATASTACK[vm->sp], val)

#define PEEK16() ({    uint8_t var = *(uint8_t*)&CODEBUF[vm->pc]; vm->pc += 1; var; })
#define PEEK16() ({    uint16_t var = *(uint16_t*)&CODEBUF[vm->pc]; vm->pc += 2; var; })
#define PEEK32() ({    uint32_t var = *(uint32_t*)&CODEBUF[vm->pc]; vm->pc += 4; var; })
#define PEEKFLT() ({   float    var = ((union int_float*)&CODEBUF[vm->pc])->f; vm->pc += 4; var; })
#define GET32() *(uint32_t*)&CODEBUF[vm->pc]
#define GET8() *(uint32_t*)&CODEBUF[vm->pc]
#define SET_PC(val) vm->pc = (val)
#define PC() vm->pc
#define SKIP_PC(cnt) (vm->pc += (cnt))

#define LOC_VARS vm->loc_var_pages[vm->call_depth]

#define DEF_INS(name) \
    } \
    static void ins_##name(vm_state_t* vm) \
    { \
        saved_pc = PC();
#define EXEC_NEXT_INSTRUCTION() \
    do { \
    op_callback callback = instructions[CODEBUF[vm->pc++]]; \
    callback(vm); \
    return; \
    } while (0)

typedef void(*op_callback)(vm_state_t*);
extern op_callback instructions[256];

#else

#define CODEBUF vm->exec_buffer
#define DATASTACK vm->data_stack
#define PUSH_STACK(val) do { var_t loc = val; --sp_ptr; *sp_ptr = loc; stktop = loc; } while (0)
#define POP_STACK()     ({var_t old = stktop; ++sp_ptr; stktop = *sp_ptr; old;})
#define STACK_TOP_VALUE() (stktop)
#define SET_STACK_TOP_VAL(val) do { uint32_t stv_loc = val; VAR_SET_VAL(stktop, stv_loc); VAR_SET_VAL(*sp_ptr, stv_loc); } while (0)

#define PEEK8() ({     int8_t var = *(int8_t*)pc_ptr; pc_ptr += 1; var; })
#define PEEK16() ({    uint16_t var = *(uint16_t*)pc_ptr; pc_ptr += 2; var; })
#define PEEK32() ({    uint32_t var = *(uint32_t*)pc_ptr; pc_ptr += 4; var; })
#define PEEKFLT() ({   float    var = ((union int_float*)pc_ptr)->f; pc_ptr += 4; var; })
#define GET32() *(uint32_t*)pc_ptr
#define GET8() *(uint8_t*)pc_ptr
#define SET_PC(val) pc_ptr = ((val) + CODEBUF)
#define PC() (pc_ptr - CODEBUF)
#define SKIP_PC(cnt) (pc_ptr += (cnt))

#define LOC_VARS current_var_page
#define DEF_INS(name) \
    ins_##name: \
    saved_pc = PC();
#define EXEC_NEXT_INSTRUCTION() \
    do { \
    goto *instructions[*pc_ptr++]; \
    } while (0)

#endif

#ifdef USE_COMPUTED_GOTOS
#define error(fmt, ...) do { \
vm->sp = sp_ptr - DATASTACK; \
vm->pc = saved_pc-1; \
error_impl(vm, fmt, ##__VA_ARGS__); \
} while (0)
#else
#define error(fmt, ...) \
do { \
vm->pc = saved_pc-1; \
error_impl(vm, fmt, ##__VA_ARGS__); \
} while (0)
#endif

__attribute__((noreturn)) static void error_impl(vm_state_t* vm, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "error at adress 0x%x, opcode 0x%x : ", vm->pc, vm->exec_buffer[vm->pc]);
    vfprintf(stderr, fmt, args);
    printf("\n");

    va_end(args);

    exit(1);
}

#ifndef USE_COMPUTED_GOTOS
static uint32_t saved_pc;
#endif

void run(vm_state_t *vm)
{
    // execute the first instruction

#ifndef USE_COMPUTED_GOTOS


    op_callback callback = instructions[CODEBUF[vm->pc++]];

    callback(vm);
#else
    register uint32_t saved_pc;
    //const uint8_t* const loc_codebuf = vm->exec_buffer;
    //var_t* const loc_datastack = vm->data_stack;
    register uint8_t* pc_ptr = vm->exec_buffer;
    register var_t stktop;
    register var_t* current_var_page = vm->loc_var_pages[0];
    register var_t* sp_ptr = vm->data_stack + vm->sp;

    static void* instructions[256] =
        {
            [0 ... 255] = &&ins_invalid,

#define INSTRUCTION(code, op) \
            [code] = &&ins_##op,
#include "instructions.h"
#undef INSTRUCTION
            };

    goto *instructions[*pc_ptr++];
#endif

DEF_INS(brk)
{
    EXEC_NEXT_INSTRUCTION();
}


DEF_INS(chknotnul)
{
    var_t var = STACK_TOP_VALUE();
    if (VAR_TYPE(var) == NULLVAL)
        error("value is null");
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(isnull)
{
    var_t val = POP_STACK();
    var_t result = MK_VAR(VAR_TYPE(val) == NULLVAL, INT);
    PUSH_STACK(result);
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(nop)
{
    // litteraly do nothing.
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pop)
{
    POP_STACK();

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(dup)
{
    PUSH_STACK(STACK_TOP_VALUE());

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushi)
{
    uint32_t val = PEEK32();

    PUSH_STACK(MK_VAR(val, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushib)
{
    int32_t val = PEEK8();

    //printf("pushing byte %d\n", val);
    var_t var = MK_VAR((uint32_t)(val), INT);
    PUSH_STACK(var);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushf)
{
    float val = PEEKFLT();

    PUSH_STACK(MK_FLT(val));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushs)
{
    uint16_t idx = PEEK16();

    if (idx >= vm->string_table_size)
        error("invalid string table index");

    unsigned object = alloc_memory_region(vm, vm->string_table[idx].str_len);
    var_t var = MK_STR(object);

    for (unsigned i = 0; i < vm->string_table[idx].str_len; ++i)
    {
        OBJ_MEM(object, i) = MK_VAR(vm->string_table[idx].str[i], INT);
    }
    PUSH_STACK(var);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushl)
{
    uint16_t var = PEEK16();

    PUSH_STACK(LOC_VARS[var]);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushg)
{
    uint16_t var = PEEK16();

    PUSH_STACK(vm->glob_vars[var]);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(movl)
{
    uint16_t var = PEEK16();

    LOC_VARS[var] = POP_STACK();

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(movg)
{
    uint16_t var = PEEK16();

    vm->glob_vars[var] = POP_STACK();

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(copyl)
{
    uint16_t var = PEEK16();

    //printf("pushing copy loc %d\n", VAR_VAL(stktop));
    LOC_VARS[var] = STACK_TOP_VALUE();

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(getaddrl)
{
    uint16_t var = PEEK16();

    var_t ptr = MK_VAR(var, LVAR_PTR);
    ptr |= MK_VAR_OBJ(vm->call_depth); // need the call depth information as well for lvar ptrs
    PUSH_STACK(ptr);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(getaddrg)
{
    uint16_t var = PEEK16();

    PUSH_STACK(MK_VAR(var, GVAR_PTR));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(cmov)
{
    var_t no = POP_STACK();
    var_t yes = POP_STACK();
    var_t cond = POP_STACK();

    if (VAR_TYPE(cond) != INT)
        error("invalid cmov type");

    var_t result = BOOL_TEST(cond) ? yes : no;
    PUSH_STACK(result);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(pushnull)
{
    PUSH_STACK(MK_VAR(0, NULLVAL));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(add)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    int32_t result = VAR_VAL(var1) + VAR_VAL(var2);
    var_t var;
    if (VAR_TYPE(var1) == PTR)
    {
        var = var1;
    }
    else if (VAR_TYPE(var2) == PTR)
    {
        var = var2;
    }
    else
        var = MK_VAR(0, INT);
    VAR_SET_VAL(var, result);
    PUSH_STACK(var);

    EXEC_NEXT_INSTRUCTION();
}


DEF_INS(sub)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    int32_t result = VAR_VAL(var1) - VAR_VAL(var2);
    var_t var;
    if (VAR_TYPE(var1) == PTR)
    {
        var = var1;
    }
    else if (VAR_TYPE(var2) == PTR)
    {
        var = var2;
    }
    else
        var = MK_VAR(0, INT);
    VAR_SET_VAL(var, result);
    PUSH_STACK(var);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(mul)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("imul of invalid types");

    int32_t result = VAR_VAL(var1) * VAR_VAL(var2);
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(inc)
{
    SET_STACK_TOP_VAL(VAR_VAL(STACK_TOP_VALUE())+1);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(dec)
{
    SET_STACK_TOP_VAL(VAR_VAL(STACK_TOP_VALUE())-1);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(incl)
{
    uint16_t var = PEEK16();

    // fast increment
    if (LOC_VARS[var] != MK_VAR(0xFFFFFFFF, INT))
        ++LOC_VARS[var];
    else // overflow
        VAR_SET_VAL(LOC_VARS[var], VAR_VAL(LOC_VARS[var])+1);

    // incl is often followed by jmp, which often lands on a pushl
    if (GET8() == 0x32) // jmp
    {
        SKIP_PC(1);
        uint32_t branch = GET32();
        SET_PC(branch);


        if (GET8() == 0x11) // pushi
        {
            SKIP_PC(1);
            uint32_t val = PEEK32();

            PUSH_STACK(MK_VAR(val, INT));

            EXEC_NEXT_INSTRUCTION();
        }
        if (GET8() == 0x14) // pushl
        {
            SKIP_PC(1);
            uint16_t var = PEEK16();

            PUSH_STACK(LOC_VARS[var]);

            EXEC_NEXT_INSTRUCTION();
        }

        EXEC_NEXT_INSTRUCTION();
    }

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(decl)
{
    uint16_t var = PEEK16();

    VAR_SET_VAL(LOC_VARS[var], VAR_VAL(LOC_VARS[var])-1);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(shl)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    uint32_t result = VAR_VAL(var1) << (VAR_VAL(var2)&0b11111); // only up to 32-bits of shift
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(shr)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    uint32_t result = VAR_VAL(var1) >> (VAR_VAL(var2)&0b11111); // only up to 32-bits of shift
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(cvtf2i)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != FLOAT)
        error("not a float\n");

    int32_t i = (int32_t)VAR_VAL_FLT(var);

    PUSH_STACK(MK_VAR(i, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(cvti2f)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != INT)
        error("not an int\n");

    float f = (float)(int32_t)VAR_VAL(var);

    PUSH_STACK(MK_FLT(f));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(idiv)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("idiv of invalid types");
    if (VAR_VAL(var2) == 0)
        error("division by zero");

    int32_t result = VAR_VAL(var1) / VAR_VAL(var2);
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(mod)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("modulo of invalid types");
    if (VAR_VAL(var2) == 0)
        error("division by zero");

    int32_t result = VAR_VAL(var1) % VAR_VAL(var2);
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(eq)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    //printf("cmp %d and %d\n", VAR_VAL(var1), VAR_VAL(var2));
    int32_t result = var1 == var2;
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(neq)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    int32_t result = var1 != var2;
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(lt)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    /*
    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("comparison of invalid types");
    */

    int32_t result = VAR_VAL(var1) < VAR_VAL(var2);


    // some sort of cmp-jcc macrofusing
    if (GET8() == 0x31) // jf
    {
        SKIP_PC(1);
        uint32_t branch = GET32();
        if (result == 0)
            SET_PC(branch);
        else
            SKIP_PC(4);

        EXEC_NEXT_INSTRUCTION();
    }
    else if (GET8() == 0x30) // jt
    {
        SKIP_PC(1);
        uint32_t branch = GET32();
        if (result != 0)
            SET_PC(branch);
        else
            SKIP_PC(4);

        EXEC_NEXT_INSTRUCTION();
    }


    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(eql)
{
    var_t var2 = POP_STACK();
    uint16_t var1 = PEEK16();

    int32_t result = LOC_VARS[var1] == var2;
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(neql)
{
    var_t var2 = POP_STACK();
    uint16_t var1 = PEEK16();

    int32_t result = LOC_VARS[var1] != var2;
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(ltl)
{
    var_t var2 = POP_STACK();
    uint16_t var1 = PEEK16();

    /*
    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("comparison of invalid types");
    */

    int32_t result = VAR_VAL(LOC_VARS[var1]) < VAR_VAL(var2);


    // some sort of cmp-jcc macrofusing
    if (GET8() == 0x31) // jf
    {
        SKIP_PC(1);
        uint32_t branch = GET32();
        if (result == 0)
            SET_PC(branch);
        else
            SKIP_PC(4);

        EXEC_NEXT_INSTRUCTION();
    }
    else if (GET8() == 0x30) // jt
    {
        SKIP_PC(1);
        uint32_t branch = GET32();
        if (result != 0)
            SET_PC(branch);
        else
            SKIP_PC(4);

        EXEC_NEXT_INSTRUCTION();
    }

    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(land)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    /*
    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("comparison of invalid types");
    */

    int32_t result = VAR_VAL(var1) && VAR_VAL(var2);

    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(lor)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    /*
    if (VAR_TYPE(var1) != INT || VAR_TYPE(var2) != INT)
        error("comparison of invalid types");
    */

    int32_t result = VAR_VAL(var1) || VAR_VAL(var2);


    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(lnot)
{
    var_t var = POP_STACK();

    if (VAR_TYPE(var) != INT)
        error("lnot invalid type");

    int32_t result = !BOOL_TEST(var);

    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(feq)
{
    var_t var2 = POP_STACK();
    var_t var1 = POP_STACK();

    if (VAR_TYPE(var1) != FLOAT || VAR_TYPE(var2) != FLOAT)
        error("comparison of invalid types");

    float lhs = VAR_VAL_FLT(var1);
    float rhs = VAR_VAL_FLT(var2);

    //printf("cmp %f and %f\n", lhs, rhs);

    int32_t result = fabsf(lhs - rhs) < FEQ_EPSILON;
    PUSH_STACK(MK_VAR(result, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(alloc)
{
    var_t size = POP_STACK();
    if (VAR_TYPE(size) != INT)
        error("alloc size is not an integer");

    unsigned object = alloc_memory_region(vm, VAR_VAL(size));
    PUSH_STACK(MK_PTR(object));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(stackcpy)
{
    var_t* data_ptr;
    uint32_t len = PEEK32();
#if defined(USE_COMPUTED_GOTOS)
    data_ptr = sp_ptr;
    sp_ptr += len;
    stktop = *sp_ptr;
#else
    static var_t stackcpy_buffer[STACK_DEPTH];
    for (uint32_t i = 0; i < len; ++i)
    {
        stackcpy_buffer[i] = POP_STACK();
    }
    data_ptr = stackcpy_buffer;
#endif

    var_t dest = POP_STACK();

    if (VAR_TYPE(dest) < PTR)
        error("invalid stack copy destination type");

    memcpy(&vm->mem_regions[VAR_OBJECT(dest)].base[VAR_OFFSET(dest)],
           data_ptr, len * sizeof(var_t));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(find)
{
    var_t var_array  = POP_STACK();
    var_t var_elem   = POP_STACK();
    if (VAR_TYPE(var_array) < PTR)
        error("invalid 'find' argument types");
    uint32_t mem_size = vm->mem_regions[VAR_OBJECT(var_array)].size;
    for (uint32_t i = 0; i < mem_size; ++i)
    {
        if (var_elem == vm->mem_regions[VAR_OBJECT(var_array)].base[i])
        {
            PUSH_STACK(MK_VAR(i, INT));
            EXEC_NEXT_INSTRUCTION();
        }
    }

    // nothing found
    PUSH_STACK(MK_VAR(-1, INT));
    EXEC_NEXT_INSTRUCTION();
}

// indirect find
DEF_INS(findi)
{
    var_t var_stride = POP_STACK();
    var_t var_array  = POP_STACK();
    var_t var_elem   = POP_STACK();
    uint32_t stride = VAR_VAL(var_stride);
    if (VAR_TYPE(var_stride) != INT || VAR_TYPE(var_array) < PTR || VAR_TYPE(var_elem) < PTR)
        error("invalid 'findi' argument types");
    uint32_t mem_size = vm->mem_regions[VAR_OBJECT(var_array)].size;
    if (mem_size % stride != 0)
        error("invalid 'findi' stride size");
    for (uint32_t i = 0; i < mem_size; ++i)
    {
        if (memcmp(&vm->mem_regions[VAR_OBJECT(var_array)].base[i*stride],
                   &vm->mem_regions[VAR_OBJECT(var_elem)].base[VAR_OFFSET(var_elem)], stride) == 0)
        {
            PUSH_STACK(MK_VAR(i, INT));
            EXEC_NEXT_INSTRUCTION();
        }
    }

    // nothing found
    PUSH_STACK(MK_VAR(-1, INT));
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(mkrange)
{
    var_t var_upper  = POP_STACK();
    var_t var_lower   = POP_STACK();
    if (VAR_TYPE(var_upper) != INT || VAR_TYPE(var_lower) != INT)
        error("invalid 'mkrange' argument types");
    uint32_t lower = VAR_VAL(var_lower);
    uint32_t upper = VAR_VAL(var_upper);
    uint32_t range = lower > upper ? 0 : upper-lower + 1;

    uint32_t new_obj = alloc_memory_region(vm, range);
    for (uint32_t i = 0; i < range; ++i)
    {
        vm->mem_regions[new_obj].base[i] = MK_VAR(lower + i, INT);
    }

    PUSH_STACK(MK_PTR(new_obj));
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(randi)
{
    var_t limit_var = POP_STACK();
    if (VAR_TYPE(limit_var) != INT)
        error("invalid randi limit type\n");

    // 64-bit to avoid possible overflows which would lead to division-by-zero errors
    int64_t signed_limit = (int32_t)VAR_VAL(limit_var);

    uint32_t val;
    const int neg = signed_limit < 0;
    uint64_t unsigned_limit = neg ? -signed_limit : signed_limit;

    val = vm_rand(vm) % (unsigned_limit+1);
    if (neg)
        val = -val;

    PUSH_STACK(MK_VAR(val, INT));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(randf)
{
    var_t limit_var = POP_STACK();
    if (VAR_TYPE(limit_var) != FLOAT)
        error("invalid randf limit type\n");

    float limit = VAR_VAL_FLT(limit_var);

    float val = (float)vm_rand(vm)/(float)(VM_RAND_MAX/limit);

    PUSH_STACK(MK_FLT(val));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(randa)
{
    var_t object = POP_STACK();
    if (VAR_TYPE(object) < PTR)
        error("invalid randa limit type\n");

    memory_region_t* reg = &vm->mem_regions[VAR_OBJECT(object)];

    uint32_t limit = reg->size;
    var_t var;
    if (limit == 0)
        var = MK_VAR(0, INT);
    else
    {
        uint32_t idx = vm_rand(vm) % limit;
        var = reg->base[idx];
    }

    PUSH_STACK(var);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(copy)
{
    var_t size = POP_STACK();
    var_t src  = POP_STACK();
    var_t dest = POP_STACK();

    //printf("copying 0x%x:0x%x to 0x%x:0x%x (size %d)\n", VAR_OBJECT(src), VAR_OFFSET(src), VAR_OBJECT(dest), VAR_OFFSET(dest), VAR_VAL(size));

    if (VAR_TYPE(size) != INT || VAR_TYPE(src) < PTR || VAR_TYPE(dest) < PTR)
        error("invalid copy type data");
    if (VAR_TYPE(src) != VAR_TYPE(dest))
        error("invalid copy type data");

    memcpy(&vm->mem_regions[VAR_OBJECT(dest)].base[VAR_OFFSET(dest)],
           &vm->mem_regions[VAR_OBJECT(src)].base[VAR_OFFSET(src)], VAR_VAL(size) * sizeof(var_t));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(load)
{
    var_t ptr = POP_STACK();
    if (VAR_TYPE(ptr) < LVAR_PTR)
        error("load addr is not a pointer nor a string");

    var_t result;
    if (VAR_TYPE(ptr) == PTR || VAR_TYPE(ptr) == STR)
        result = vm->mem_regions[VAR_OBJECT(ptr)].base[VAR_OFFSET(ptr)];
    else if (VAR_TYPE(ptr) == LVAR_PTR)
        result = vm->loc_var_pages[VAR_OBJECT(ptr)][VAR_OFFSET(ptr)];
    else // if (VAR_TYPE(ptr) == GVAR_PTR)
        result = vm->glob_vars[VAR_OFFSET(ptr)];

    PUSH_STACK(result);

    //printf("loading 0x%x => %d\n", VAR_OFFSET(ptr), VAR_VAL(result));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(store)
{
    var_t val = POP_STACK();
    var_t ptr = POP_STACK();
    if (VAR_TYPE(ptr) != PTR && VAR_TYPE(ptr) != LVAR_PTR && VAR_TYPE(ptr) != GVAR_PTR)
        error("store addr is not a pointer");

    //printf("storing 0x%x with %d\n", VAR_OFFSET(ptr), VAR_VAL(val));

    if (VAR_TYPE(ptr) == PTR)
        vm->mem_regions[VAR_OBJECT(ptr)].base[VAR_OFFSET(ptr)] = val;
    else if (VAR_TYPE(ptr) == LVAR_PTR)
        vm->loc_var_pages[VAR_OBJECT(ptr)][VAR_OFFSET(ptr)] = val;
    else // if (VAR_TYPE(ptr) == GVAR_PTR)
        vm->glob_vars[VAR_OFFSET(ptr)] = val;

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(memsize)
{
    var_t val = POP_STACK();
    if (VAR_TYPE(val) != PTR)
        error("not a memory zone");

    var_t size = MK_VAR(vm->mem_regions[VAR_OBJECT(val)].size, INT);
    PUSH_STACK(size);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(memresize)
{
    var_t size = POP_STACK();
    var_t val = POP_STACK();

    if (VAR_TYPE(size) != INT)
        error("size is not an integer");
    if (VAR_TYPE(val) != PTR)
        error("not a memory zone");

    resize_memory_region(vm, VAR_OBJECT(val), VAR_VAL(size));

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(arraycat)
{
    var_t rhs = POP_STACK();
    var_t lhs = POP_STACK();
    if (VAR_TYPE(lhs) != PTR || VAR_TYPE(rhs) != PTR)
        error("not a memory zone");
    if (VAR_OFFSET(lhs) != 0 || VAR_OFFSET(rhs) != 0)
        error("not arrays");

    uint32_t rhs_size = vm->mem_regions[VAR_OBJECT(rhs)].size;
    uint32_t lhs_size = vm->mem_regions[VAR_OBJECT(lhs)].size;

    unsigned new_obj = alloc_memory_region(vm, lhs_size+rhs_size);
    var_t new = MK_PTR(new_obj);

    memcpy(vm->mem_regions[new_obj].base, vm->mem_regions[VAR_OBJECT(lhs)].base, lhs_size*sizeof(var_t));
    memcpy(vm->mem_regions[new_obj].base+lhs_size, vm->mem_regions[VAR_OBJECT(rhs)].base, rhs_size*sizeof(var_t));

    PUSH_STACK(new);

    EXEC_NEXT_INSTRUCTION();
}


DEF_INS(jt)
{
    var_t val = POP_STACK();
    /*
    if (VAR_TYPE(val) != INT && VAR_TYPE(val) != PTR)
        error("jump test data is not an integer nor a pointer");
    */

    uint32_t branch = GET32();
    if (BOOL_TEST(val))
        SET_PC(branch);
    else
        SKIP_PC(4);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(jf)
{
    var_t val = POP_STACK();
    /*
    if (VAR_TYPE(val) != INT && VAR_TYPE(val) != PTR)
        error("jump test data is not an integer nor a pointer");
    */

    uint32_t branch = GET32();
    if (!BOOL_TEST(val))
        SET_PC(branch);
    else
        SKIP_PC(4);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(jmp)
{
    uint32_t branch = GET32();
    SET_PC(branch);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(call)
{
    if (vm->call_depth >= MAX_CALL_DEPTH)
        abort();

    uint32_t branch = PEEK32();

#ifndef USE_COMPUTED_GOTOS
    vm->call_stack[vm->call_depth] = PC();
#else
    vm->call_stack[vm->call_depth] = pc_ptr;
#endif

    ++vm->call_depth;

#ifdef USE_COMPUTED_GOTOS
    LOC_VARS = vm->loc_var_pages[vm->call_depth];
#endif

    // clear the var memory
    memset(LOC_VARS, 0, VAR_COUNT*sizeof(var_t));

    SET_PC(branch);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(calli)
{
    if (vm->call_depth >= MAX_CALL_DEPTH)
        abort();

    var_t target = POP_STACK();
    if (VAR_TYPE(target) != INT)
        error("invalid indirect call target");

    uint32_t branch = VAR_VAL(target);

#ifndef USE_COMPUTED_GOTOS
    vm->call_stack[vm->call_depth] = PC();
#else
    vm->call_stack[vm->call_depth] = pc_ptr;
#endif

    ++vm->call_depth;
#ifdef USE_COMPUTED_GOTOS
    LOC_VARS = vm->loc_var_pages[vm->call_depth];
#endif

    SET_PC(branch);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(ret)
{
    --vm->call_depth;

    if (vm->call_depth == -1) // end program
    {
        //abort();
        vm->stopped = 1;
        return;
    }

#ifdef USE_COMPUTED_GOTOS
    LOC_VARS = vm->loc_var_pages[vm->call_depth];
    pc_ptr = vm->call_stack[vm->call_depth];
#else
    SET_PC(vm->call_stack[vm->call_depth]);
#endif

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(strlen)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != STR)
        error("expected a string");

    assert(VAR_OFFSET(var) == 0);

    var_t size = MK_VAR(vm->mem_regions[VAR_OBJECT(var)].size, INT);
    PUSH_STACK(size);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(strcat)
{
    var_t rhs = POP_STACK();
    var_t lhs = POP_STACK();
    if (VAR_TYPE(lhs) != STR || VAR_TYPE(rhs) != STR)
        error("expected a string");

    assert(VAR_OFFSET(lhs) == 0);
    assert(VAR_OFFSET(rhs) == 0);

    uint32_t rhs_size = vm->mem_regions[VAR_OBJECT(rhs)].size;
    uint32_t lhs_size = vm->mem_regions[VAR_OBJECT(lhs)].size;

    unsigned new_obj = alloc_memory_region(vm, lhs_size+rhs_size);
    var_t new = MK_STR(new_obj);

    memcpy(vm->mem_regions[new_obj].base, vm->mem_regions[VAR_OBJECT(lhs)].base, lhs_size*sizeof(var_t));
    memcpy(vm->mem_regions[new_obj].base+lhs_size, vm->mem_regions[VAR_OBJECT(rhs)].base, rhs_size*sizeof(var_t));

    PUSH_STACK(new);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(stradd)
{
    var_t rhs = POP_STACK();
    var_t lhs = POP_STACK();
    if (VAR_TYPE(lhs) != STR)
        error("expected a string");
    if (VAR_TYPE(rhs) != INT)
        error("expected a char");

    assert(VAR_OFFSET(lhs) == 0);

    uint32_t lhs_size = vm->mem_regions[VAR_OBJECT(lhs)].size;

    unsigned new_obj = alloc_memory_region(vm, lhs_size+1);
    var_t new = MK_STR(new_obj);

    memcpy(vm->mem_regions[new_obj].base, vm->mem_regions[VAR_OBJECT(lhs)].base, lhs_size*sizeof(var_t));
    vm->mem_regions[new_obj].base[lhs_size] = rhs;

    PUSH_STACK(new);

    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(streq)
{
    var_t rhs = POP_STACK();
    var_t lhs = POP_STACK();
    if (VAR_TYPE(lhs) != STR || VAR_TYPE(rhs) != STR)
        error("expected a string");

    assert(VAR_OFFSET(lhs) == 0);
    assert(VAR_OFFSET(rhs) == 0);

    uint32_t rhs_size = vm->mem_regions[VAR_OBJECT(rhs)].size;
    uint32_t lhs_size = vm->mem_regions[VAR_OBJECT(lhs)].size;

    int result;
    if (lhs_size != rhs_size)
        result = 0;
    else
    {
        result = 1;
        for (unsigned i = 0; i < lhs_size; ++i)
        {
            if (VAR_VAL(OBJ_MEM(VAR_OBJECT(lhs), i)) !=
                VAR_VAL(OBJ_MEM(VAR_OBJECT(rhs), i)))
            {
                result = 0;
                break;
            }
        }
    }

    var_t new = MK_VAR(result, INT);

    PUSH_STACK(new);

    EXEC_NEXT_INSTRUCTION();
}

#include "interp_math_impl.h"

DEF_INS(syscall)
{
    uint32_t id = PEEK32();

#ifdef USE_COMPUTED_GOTOS
    vm->sp = sp_ptr - DATASTACK;
    vm->pc = pc_ptr - CODEBUF;
#endif
    vm_syscall(vm, id);
#ifdef USE_COMPUTED_GOTOS
    sp_ptr = DATASTACK + vm->sp;
    stktop = *sp_ptr;
    pc_ptr = CODEBUF + vm->pc;
#endif

    if (!vm->stopped)
        EXEC_NEXT_INSTRUCTION();
}

DEF_INS(invalid)
{
    uint8_t opcode = CODEBUF[PC()-1];
    error("unknown opcode 0x%x, addr 0x%x\n", opcode, PC());
}

#ifdef USE_COMPUTED_GOTOS
}
#else
}

    op_callback instructions[256] =
        {
            [0 ... 255] = ins_invalid,

#define INSTRUCTION(code, op) \
            [code] = ins_##op,
#include "instructions.h"
#undef INSTRUCTION
            };

#endif
