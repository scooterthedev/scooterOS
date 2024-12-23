#include "../include/compiler.h"

static token_t* current_token;
static lexer_t* current_lexer;

static void advance_token() {
    current_token = lexer_next_token(current_lexer);
}

parser_t* parser_create(lexer_t* lexer) {
    parser_t* parser = kmalloc(sizeof(parser_t));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    current_lexer = lexer;
    current_token = lexer_next_token(lexer);
    parser->current_token = current_token;
    
    return parser;
}

void parser_destroy(parser_t* parser) {
    if (parser) {
        kfree(parser);
    }
}

static ast_node_t* parse_number() {
    ast_node_t* node = kmalloc(sizeof(ast_node_t));
    node->type = NODE_NUMBER;
    node->number_value = atoi(current_token->value);
    node->left = NULL;
    node->right = NULL;
    advance_token();
    return node;
}

ast_node_t* parser_parse(parser_t* parser) {
    ast_node_t* root = kmalloc(sizeof(ast_node_t));
    root->type = NODE_PROGRAM;
    root->left = parse_number();
    root->right = NULL;
    root->next = NULL;
    return root;
} 