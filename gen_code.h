#ifndef _GEN_CODE_H
#define _GEN_CODE_H
#include "ast.h"
#include "bof.h"

extern void gen_code_initialize(); 
extern void gen_code_program(BOFFILE bf, block_t prog);

#endif