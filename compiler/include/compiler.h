#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../memory.h"
#include "../string.h"

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQUALS,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE
} token_type_t;

// Token structure
typedef struct {
    token_type_t type;
    char* value;
    size_t line;
    size_t column;
} token_t;

// Lexer structure
typedef struct {
    char* source;
    size_t position;
    size_t line;
    size_t column;
} lexer_t;

// AST node types
typedef enum {
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_FUNCTION_CALL,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_RETURN_STMT,
    NODE_PROGRAM,
    NODE_BINARY_EXPR
} node_type_t;

// Binary operator types
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} binary_op_t;

// AST node structure
typedef struct ast_node {
    node_type_t type;
    char* value;
    int number_value;
    binary_op_t op;
    struct ast_node* left;
    struct ast_node* right;
    struct ast_node* next;
} ast_node_t;

// Parser structure
typedef struct {
    lexer_t* lexer;
    token_t* current_token;
} parser_t;

// IR instruction types
typedef enum {
    IR_LOAD,
    IR_STORE,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_JUMP,
    IR_JUMP_IF,
    IR_LABEL,
    IR_RETURN
} ir_op_t;

// IR instruction structure
typedef struct ir_instr {
    ir_op_t op;
    char* dest;
    char* src1;
    char* src2;
    int immediate;
    struct ir_instr* next;
} ir_instr_t;

// Function declarations
lexer_t* lexer_create(char* source);
token_t* lexer_next_token(lexer_t* lexer);
void lexer_destroy(lexer_t* lexer);
parser_t* parser_create(lexer_t* lexer);
void parser_destroy(parser_t* parser);
ast_node_t* parser_parse(parser_t* parser);
ir_instr_t* generate_ir(ast_node_t* ast);
void generate_assembly(ir_instr_t* ir, char* output);

#endif