#include "dan_pa_vm.h"

void _dpvm_priv_unknown_op(DanPaVM* vm);

void _dpvm_priv_SYSCALL_op(DanPaVM* vm);
void _dpvm_priv_BRT_op(DanPaVM* vm);
void _dpvm_priv_BRF_op(DanPaVM* vm);
void _dpvm_priv_JMP_op(DanPaVM* vm);
void _dpvm_priv_CALL_op(DanPaVM* vm);
void _dpvm_priv_RET_op(DanPaVM* vm);

void _dpvm_priv_LDLOC_op(DanPaVM* vm);
void _dpvm_priv_LDGLB_op(DanPaVM* vm);
void _dpvm_priv_STLOC_op(DanPaVM* vm);
void _dpvm_priv_STGLB_op(DanPaVM* vm);
void _dpvm_priv_ARLD_op(DanPaVM* vm);
void _dpvm_priv_ARST_op(DanPaVM* vm);
void _dpvm_priv_ARLEN_op(DanPaVM* vm);
void _dpvm_priv_ARRESIZE_op(DanPaVM* vm);

void _dpvm_priv_ADDI_op(DanPaVM* vm);
void _dpvm_priv_SUBI_op(DanPaVM* vm);
void _dpvm_priv_AND_op(DanPaVM* vm);
void _dpvm_priv_OR_op(DanPaVM* vm);
void _dpvm_priv_XOR_op(DanPaVM* vm);
void _dpvm_priv_NOT_op(DanPaVM* vm);
void _dpvm_priv_SHL_op(DanPaVM* vm);
void _dpvm_priv_SHR_op(DanPaVM* vm);
void _dpvm_priv_LSR_op(DanPaVM* vm);
void _dpvm_priv_NEGI_op(DanPaVM* vm);
void _dpvm_priv_MULI_op(DanPaVM* vm);
void _dpvm_priv_DIVI_op(DanPaVM* vm);
void _dpvm_priv_MODI_op(DanPaVM* vm);
void _dpvm_priv_INC_op(DanPaVM* vm);
void _dpvm_priv_DEC_op(DanPaVM* vm);

void _dpvm_priv_ADDF_op(DanPaVM* vm);
void _dpvm_priv_SUBF_op(DanPaVM* vm);
void _dpvm_priv_NEGF_op(DanPaVM* vm);
void _dpvm_priv_MULF_op(DanPaVM* vm);
void _dpvm_priv_DIVF_op(DanPaVM* vm);
void _dpvm_priv_MODF_op(DanPaVM* vm);

void _dpvm_priv_PUSH_op(DanPaVM* vm);
void _dpvm_priv_SWAP_op(DanPaVM* vm);
void _dpvm_priv_DUP_op(DanPaVM* vm);

void _dpvm_priv_I2F_op(DanPaVM* vm);
void _dpvm_priv_F2I_op(DanPaVM* vm);
void _dpvm_priv_I2C_op(DanPaVM* vm);
void _dpvm_priv_C2I_op(DanPaVM* vm);

void _dpvm_priv_DBG_op(DanPaVM* vm);
