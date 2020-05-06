#ifndef INTERP_IMPL // this is just to make the IDE's code analyzer stay calm
#include "interp.c"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEF_1OP_FMATH_INS(func) \
DEF_INS(func) \
{ \
    var_t var = POP_STACK(); \
    if (VAR_TYPE(var) != FLOAT) \
        error("expected a float"); \
    float result = func##f(VAR_VAL_FLT(var)); \
    PUSH_STACK(MK_FLT(result)); \
    EXEC_NEXT_INSTRUCTION(); \
}

#define DEF_2OP_FMATH_INS(func) \
DEF_INS(func) \
{ \
    var_t var2 = POP_STACK(); \
    var_t var1 = POP_STACK(); \
    if (VAR_TYPE(var1) != FLOAT || VAR_TYPE(var2) != FLOAT) \
        error("expected floats"); \
    float result = func##f(VAR_VAL_FLT(var1), VAR_VAL_FLT(var2)); \
    PUSH_STACK(MK_FLT(result)); \
    EXEC_NEXT_INSTRUCTION(); \
}

DEF_1OP_FMATH_INS(cos)
DEF_1OP_FMATH_INS(sin)
DEF_1OP_FMATH_INS(tan)
DEF_1OP_FMATH_INS(acos)
DEF_1OP_FMATH_INS(asin)
DEF_1OP_FMATH_INS(atan)
DEF_2OP_FMATH_INS(atan2)

DEF_INS(ln)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != FLOAT)
        error("expected a float");
    float result = logf(VAR_VAL_FLT(var));
    PUSH_STACK(MK_FLT(result));
    EXEC_NEXT_INSTRUCTION();
}

DEF_1OP_FMATH_INS(log10)
DEF_2OP_FMATH_INS(pow)
DEF_1OP_FMATH_INS(exp)
DEF_1OP_FMATH_INS(sqrt)
DEF_1OP_FMATH_INS(fabs)
DEF_1OP_FMATH_INS(ceil)
DEF_1OP_FMATH_INS(floor)

DEF_INS(abs)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != INT)
        error("expected an int");
    uint32_t result = abs((int32_t)VAR_VAL(var));
    PUSH_STACK(MK_VAR(result, INT));
    EXEC_NEXT_INSTRUCTION();
}

DEF_INS(rad2deg)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != FLOAT)
        error("expected a float");
    float result = VAR_VAL_FLT(var) * 180.f/M_PI;
    PUSH_STACK(MK_FLT(result));
    EXEC_NEXT_INSTRUCTION();
}
DEF_INS(deg2rad)
{
    var_t var = POP_STACK();
    if (VAR_TYPE(var) != FLOAT)
        error("expected a float");
    float result = VAR_VAL_FLT(var) * M_PI/180.f;
    PUSH_STACK(MK_FLT(result));
    EXEC_NEXT_INSTRUCTION();
}
