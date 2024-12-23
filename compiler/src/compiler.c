#include "../include/compiler.h"
#include "../../memory.h"
#include "../../string.h"

static bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

lexer_t* lexer_create(char* source) {
    lexer_t* lexer = kmalloc(sizeof(lexer_t));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    
    return lexer;
}

static token_t* create_token(token_type_t type, char* value, size_t line, size_t column) {
    token_t* token = kmalloc(sizeof(token_t));
    if (!token) return NULL;
    
    token->type = type;
    token->value = value;
    token->line = line;
    token->column = column;
    
    return token;
}

token_t* lexer_next_token(lexer_t* lexer) {
    while (is_whitespace(lexer->source[lexer->position])) {
        if (lexer->source[lexer->position] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->position++;
    }
    
    if (lexer->source[lexer->position] == '\0') {
        return create_token(TOKEN_EOF, NULL, lexer->line, lexer->column);
    }
    
    char current = lexer->source[lexer->position];
    switch (current) {
        case '+': 
            lexer->position++;
            return create_token(TOKEN_PLUS, "+", lexer->line, lexer->column++);
        case '-':
            lexer->position++;
            return create_token(TOKEN_MINUS, "-", lexer->line, lexer->column++);
        case '*':
            lexer->position++;
            return create_token(TOKEN_STAR, "*", lexer->line, lexer->column++);
        case '/':
            lexer->position++;
            return create_token(TOKEN_SLASH, "/", lexer->line, lexer->column++);
        case '=':
            lexer->position++;
            return create_token(TOKEN_EQUALS, "=", lexer->line, lexer->column++);
        case ';':
            lexer->position++;
            return create_token(TOKEN_SEMICOLON, ";", lexer->line, lexer->column++);
        case '(':
            lexer->position++;
            return create_token(TOKEN_LPAREN, "(", lexer->line, lexer->column++);
        case ')':
            lexer->position++;
            return create_token(TOKEN_RPAREN, ")", lexer->line, lexer->column++);
        case '{':
            lexer->position++;
            return create_token(TOKEN_LBRACE, "{", lexer->line, lexer->column++);
        case '}':
            lexer->position++;
            return create_token(TOKEN_RBRACE, "}", lexer->line, lexer->column++);
    }
    
    if (is_digit(current)) {
        size_t start = lexer->position;
        while (is_digit(lexer->source[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }
        
        size_t length = lexer->position - start;
        char* value = kmalloc(length + 1);
        memcpy(value, &lexer->source[start], length);
        value[length] = '\0';
        
        return create_token(TOKEN_NUMBER, value, lexer->line, lexer->column - length);
    }
    
    if (is_alpha(current)) {
        size_t start = lexer->position;
        while (is_alnum(lexer->source[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }
        
        size_t length = lexer->position - start;
        char* value = kmalloc(length + 1);
        memcpy(value, &lexer->source[start], length);
        value[length] = '\0';
        
        return create_token(TOKEN_IDENTIFIER, value, lexer->line, lexer->column - length);
    }
    
    return NULL;
}

void lexer_destroy(lexer_t* lexer) {
    if (lexer) {
        kfree(lexer);
    }
}