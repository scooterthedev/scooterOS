#include "../include/compiler.h"
#include "../../memory.h"
#include "../../string.h"

static int temp_counter = 0;
static int label_counter = 0;

static char* new_temp() {
    char* temp = kmalloc(16);
    sprintf(temp, "t%d", temp_counter++);
    return temp;
}

static char* new_label() {
    char* label = kmalloc(16);
    sprintf(label, "L%d", label_counter++);
    return label;
}

static ir_instr_t* create_ir_instr(ir_op_t op) {
    ir_instr_t* instr = kmalloc(sizeof(ir_instr_t));
    instr->op = op;
    instr->dest = NULL;
    instr->src1 = NULL;
    instr->src2 = NULL;
    instr->immediate = 0;
    instr->next = NULL;
    return instr;
}

static ir_instr_t* generate_expression(ast_node_t* node, char** result) {
    if (!node) return NULL;
    
    ir_instr_t* head = NULL;
    ir_instr_t* current = NULL;
    
    switch (node->type) {
        case NODE_NUMBER: {
            ir_instr_t* load_instr = create_ir_instr(IR_LOAD);
            load_instr->immediate = node->number_value;
            *result = new_temp();
            load_instr->dest = *result;
            head = current = load_instr;
            break;
        }
        case NODE_BINARY_EXPR: {
            char* left_result;
            char* right_result;
            
            ir_instr_t* left_code = generate_expression(node->left, &left_result);
            ir_instr_t* right_code = generate_expression(node->right, &right_result);
            
            head = left_code;
            current = head;
            while (current->next) current = current->next;
            current->next = right_code;
            
            ir_instr_t* op_instr = create_ir_instr(
                node->op == OP_ADD ? IR_ADD :
                node->op == OP_SUB ? IR_SUB :
                node->op == OP_MUL ? IR_MUL : IR_DIV
            );
            
            *result = new_temp();
            op_instr->dest = *result;
            op_instr->src1 = left_result;
            op_instr->src2 = right_result;
            
            while (current->next) current = current->next;
            current->next = op_instr;
            break;
        }
    }
    
    return head;
}

ir_instr_t* generate_ir(ast_node_t* ast) {
    if (!ast) return NULL;
    
    ir_instr_t* head = NULL;
    ir_instr_t* current = NULL;
    
    switch (ast->type) {
        case NODE_PROGRAM: {
            ast_node_t* stmt = ast->left;
            while (stmt) {
                char* result;
                ir_instr_t* stmt_code = generate_expression(stmt, &result);
                
                if (!head) {
                    head = current = stmt_code;
                } else {
                    current->next = stmt_code;
                }
                
                while (current->next) current = current->next;
                stmt = stmt->next;
            }
            break;
        }
        
    //TODO Add more cases
    }
    
    return head;
} 