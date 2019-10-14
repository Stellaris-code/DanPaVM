//OP codes
#define _NOP_       0x00
#define _SYSCALL_   0x01
#define _BRT_       0x02
#define _BRF_       0x03
#define _JMP_       0x04
#define _CALL_      0x05
#define _RET_       0x06

#define _LDLOC_     0x08
#define _LDGLB_     0x09
#define _STLOC_     0x0A
#define _STGLB_     0x0B
#define _ARLD_      0x0C
#define _ARST_      0x0D
#define _ARLEN_     0x0E
#define _ARRESIZE_  0x0F

#define _ADDI_      0x10
#define _SUBI_      0x11
#define _AND_       0x12
#define _OR_        0x13
#define _XOR_       0x14
#define _NOT_       0x15
#define _SHL_       0x16
#define _SHR_       0x17
#define _LSR_       0x18
#define _NEGI_      0x19
#define _MULI_      0x1A
#define _DIVI_      0x1B
#define _MODI_      0x1C
#define _INC_       0x1D
#define _DEC_       0x1E

#define _ADDF_      0x20
#define _SUBF_      0x21
#define _NEGF_      0x29
#define _MULF_      0x2A
#define _DIVF_      0x2B
#define _MODF_      0x2C

#define _PUSH_      0x30
#define _SWAP_      0x31
#define _DUP_       0x32

#define _I2F_       0x36
#define _F2I_       0x37
#define _I2C_       0x38
#define _C2I_       0x39

#define _DBG_       0xFF
