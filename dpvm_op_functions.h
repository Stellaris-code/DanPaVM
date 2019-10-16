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

void _dpvm_priv_DBG_op(DanPaVM* vm);
