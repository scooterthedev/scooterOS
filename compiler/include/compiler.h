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
    TOKEN_KEYWORD
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

void compiler_init(void);
lexer_t* lexer_create(char* source);
token_t* lexer_next_token(lexer_t* lexer);
void lexer_destroy(lexer_t* lexer);

#endif