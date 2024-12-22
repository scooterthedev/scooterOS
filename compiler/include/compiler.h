#ifndef COMPILER_H
#define COMPILER_H

#include <stdint.h>
#include <stdbool.h>

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
    TOKEN_RBRACE,
    TOKEN_KEYWORD,
    TOKEN_INT,
    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE
} token_type_t;

typedef struct {
    token_type_t type;
    char* value;
    size_t line;
    size_t column;
} token_t;

typedef struct {
    char* source;
    size_t position;
    size_t line;
    size_t column;
} lexer_t;

typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_IF,
    NODE_WHILE,
    NODE_BINARY_OP,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_ASSIGNMENT,
    NODE_DECLARATION
} node_type_t;

typedef struct ast_node {
    node_type_t type;
    struct ast_node* left;
    struct ast_node* right;
    struct ast_node* next;
    char* value;
    int number_value;
} ast_node_t;

typedef struct {
    lexer_t* lexer;
    token_t* current_token;
} parser_t;

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

typedef struct ir_instr {
    ir_op_t op;
    char* dest;
    char* src1;
    char* src2;
    int immediate;
    struct ir_instr* next;
} ir_instr_t;

void compiler_init(void);
lexer_t* lexer_create(char* source);
token_t* lexer_next_token(lexer_t* lexer);
void lexer_destroy(lexer_t* lexer);

parser_t* parser_create(lexer_t* lexer);
ast_node_t* parser_parse(parser_t* parser);
void parser_destroy(parser_t* parser);

ir_instr_t* generate_ir(ast_node_t* ast);
void print_ir(ir_instr_t* ir);

void generate_assembly(ir_instr_t* ir, char* output_file);

#endif