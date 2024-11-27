#include <stdio.h>
#include <stdlib.h>
#include "gen_code.h"
#include "ast.h"
#include "bof.h"

// Symbol table placeholder for variable management
typedef struct {
    char *name;
    int address;
} symbol_t;

#define MAX_SYMBOLS 1024
static symbol_t symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

// Initialize code generator resources
void gen_code_initialize() {
    symbol_count = 0; // Clear the symbol table
}

// Helper function to find a variable in the symbol table
static int find_symbol(char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1; // Not found
}

// Helper function to add a new variable to the symbol table
static int add_symbol(char *name) {
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table overflow.\n");
        exit(1);
    }
    symbol_table[symbol_count].name = name;
    symbol_table[symbol_count].address = symbol_count; // Simplistic memory mapping
    return symbol_count++;
}

// Generate code for expressions
static void gen_code_expression(BOFFILE bf, expr_t expr) {
    switch (expr->tag) {
        case EXPR_LITERAL:
            bof_emit(bf, OP_LIT, expr->literal_value);
            break;
        case EXPR_VAR: {
            int addr = find_symbol(expr->var_name);
            if (addr == -1) {
                fprintf(stderr, "Undefined variable: %s\n", expr->var_name);
                exit(1);
            }
            bof_emit(bf, OP_LOD, addr);
            break;
        }
        case EXPR_BINOP:
            gen_code_expression(bf, expr->binop.left);
            gen_code_expression(bf, expr->binop.right);
            switch (expr->binop.op) {
                case BINOP_ADD: bof_emit(bf, OP_ADD); break;
                case BINOP_SUB: bof_emit(bf, OP_SUB); break;
                case BINOP_MUL: bof_emit(bf, OP_MUL); break;
                case BINOP_DIV: bof_emit(bf, OP_DIV); break;
                default:
                    fprintf(stderr, "Unknown binary operator.\n");
                    exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unknown expression type.\n");
            exit(1);
    }
}

// Generate code for statements
static void gen_code_statement(BOFFILE bf, stmt_t stmt) {
    switch (stmt->tag) {
        case STMT_ASSIGN: {
            int addr = find_symbol(stmt->assign.var_name);
            if (addr == -1) {
                addr = add_symbol(stmt->assign.var_name);
            }
            gen_code_expression(bf, stmt->assign.expr);
            bof_emit(bf, OP_STR, addr);
            break;
        }
        case STMT_PRINT:
            gen_code_expression(bf, stmt->print.expr);
            bof_emit(bf, OP_PRINT);
            break;
        default:
            fprintf(stderr, "Unknown statement type.\n");
            exit(1);
    }
}

// Generate code for a block of statements
static void gen_code_block(BOFFILE bf, block_t block) {
    for (stmt_list_t stmt = block->stmts; stmt != NULL; stmt = stmt->next) {
        gen_code_statement(bf, stmt->stmt);
    }
}

// Entry point for program code generation
void gen_code_program(BOFFILE bf, block_t prog) {
    gen_code_initialize();
    gen_code_block(bf, prog);
    bof_emit(bf, OP_HALT); // End of program
}
