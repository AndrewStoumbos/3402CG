#include "gen_code.h"
#include "literal_table.h"
#include "vm_code.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

// Helper function to handle errors during code generation
void bail_with_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

// Function to generate code for an expression
code_seq gen_code_expr(expr_t expr)
{
    code_seq result;
    
    switch (expr->type)
    {
    case EXPR_LITERAL:
        result = code_seq_push_value(expr->value);  // Push literal value
        break;
    
    case EXPR_VARIABLE:
        result = code_seq_push_variable(expr->var_name);  // Push variable value
        break;
    
    case EXPR_BINARY_OP:
        // Recursively generate code for left and right operands
        result = gen_code_expr(expr->left_operand);
        code_seq_add(result, gen_code_expr(expr->right_operand));
        
        // Perform the binary operation (e.g., +, -, *)
        switch (expr->op)
        {
        case OP_ADD:
            result = code_seq_add(result, code_add_instruction());
            break;
        case OP_SUB:
            result = code_seq_add(result, code_sub_instruction());
            break;
        case OP_MUL:
            result = code_seq_add(result, code_mul_instruction());
            break;
        case OP_DIV:
            result = code_seq_add(result, code_div_instruction());
            break;
        default:
            bail_with_error("Unsupported binary operator");
            break;
        }
        break;
    
    default:
        bail_with_error("Unsupported expression type");
        break;
    }
    
    return result;
}

// Function to generate code for a print statement
code_seq gen_code_print(print_stmt_t print_stmt)
{
    code_seq result = gen_code_expr(print_stmt->expr);  // Generate code for the expression to be printed
    code_seq_add(result, code_push_instruction(PRINT));  // Push the PRINT operation
    return result;
}

// Function to generate code for an assignment statement
code_seq gen_code_assign(assign_stmt_t assign_stmt)
{
    code_seq result = gen_code_expr(assign_stmt->expr);  // Generate code for the expression on the right-hand side
    code_seq_add(result, code_store_variable(assign_stmt->var_name));  // Store the result in the variable
    return result;
}

// Function to generate code for a procedure declaration (not required for this assignment, but future-proof)
code_seq gen_code_proc_decl(proc_decl_t proc_decl)
{
    // Procedure code generation is not needed for this assignment as per the current instructions
    return code_seq_empty();  // Return an empty sequence
}

// Function to generate code for a program statement
code_seq gen_code_stmt(stmt_t stmt)
{
    code_seq result;
    
    switch (stmt->type)
    {
    case STMT_ASSIGN:
        result = gen_code_assign((assign_stmt_t)stmt);  // Handle assignment
        break;
    
    case STMT_PRINT:
        result = gen_code_print((print_stmt_t)stmt);  // Handle print statement
        break;
    
    default:
        bail_with_error("Unsupported statement type");
        break;
    }
    
    return result;
}

// Function to generate code for the entire program
code_seq gen_code_program(program_t program)
{
    code_seq result = code_seq_empty();  // Start with an empty code sequence
    
    // Generate code for each statement in the program
    for (int i = 0; i < program->num_statements; i++)
    {
        code_seq stmt_code = gen_code_stmt(program->statements[i]);
        result = code_seq_add(result, stmt_code);
    }
    
    // End the program with an EXIT instruction
    result = code_seq_add(result, code_exit_instruction());
    
    return result;
}

// Function to generate code for an expression
code_seq code_seq_push_value(int value)
{
    code_seq seq = code_seq_empty();
    seq = code_seq_add(seq, code_push_instruction(value));
    return seq;
}

// Function to push a variable to the code sequence
code_seq code_seq_push_variable(const char *var_name)
{
    code_seq seq = code_seq_empty();
    seq = code_seq_add(seq, code_push_variable_instruction(var_name));
    return seq;
}

// Function to store the result of an expression in a variable
code_seq code_store_variable(const char *var_name)
{
    return code_store_instruction(var_name);
}

// Function to generate binary operation code
code_seq code_add_instruction()
{
    return code_binary_op_instruction(ADD);
}

code_seq code_sub_instruction()
{
    return code_binary_op_instruction(SUB);
}

code_seq code_mul_instruction()
{
    return code_binary_op_instruction(MUL);
}

code_seq code_div_instruction()
{
    return code_binary_op_instruction(DIV);
}

// Helper functions for handling instruction generation
code_seq code_push_instruction(int value)
{
    code_seq seq = code_seq_empty();
    seq = code_seq_add(seq, generate_push_instruction(value));
    return seq;
}

code_seq code_push_variable_instruction(const char *var_name)
{
    code_seq seq = code_seq_empty();
    seq = code_seq_add(seq, generate_push_variable_instruction(var_name));
    return seq;
}

code_seq code_binary_op_instruction(opcode_t op)
{
    code_seq seq = code_seq_empty();
    seq = code_seq_add(seq, generate_binary_op_instruction(op));
    return seq;
}

code_seq code_exit_instruction()
{
    return generate_exit_instruction();
}
