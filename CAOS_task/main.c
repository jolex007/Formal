#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct {
    const char * name;
    void       * pointer;
} symbol_t;


typedef enum {
    FUNCTION,
    CONSTANT,
    OPERATION,
    END_OF_ARRAY
} type_of_token_t;


typedef struct {
    const char * name;
    type_of_token_t type;
    int num;
    int count_of_vars;
    void* func_ptr;
} token_t;

const uint32_t PUSH_lr =                0xe92d4030;
const uint32_t POP_lr =                 0xe8bd4030;

const uint32_t BX_lr =                  0xe12fff1e;

const uint32_t ALL_COMMAND_PATTERN =    0xe0000000;

const uint32_t MOVE_OPCODE =            0x01a00000;
const uint32_t SUB_OPCODE =             0x00400000;
const uint32_t ADD_OPCODE =             0x00800000;

const uint32_t STR_PREFIX =             0xe5800000;
const uint32_t LDR_PREFIX =             0xe5900000;

const uint32_t I_bit =                  0x02000000;

const uint32_t ADD_IP_FIRST =           0xe28fc600;
const uint32_t ADD_IP_SECOND =          0xe28cca10;
const uint32_t LDR_COMMAND =            0xe5bcfca4;

const uint32_t REG_SHIFTER_OPER =       0x00000090;

const uint32_t BX_R4 =                  0xe12fff14;


uint32_t get_reg(uint32_t num, int is_rd) {
    if (is_rd) {
        return (num << 12);
    } else {
        return (num << 16);
    }
}

uint32_t get_rs(uint32_t num) {
    return (num << 8);
}

uint32_t get_shifter_oper(uint32_t num) {
    return num;
}

uint32_t get_shifter_com(uint32_t num) {
    return num;
}

uint32_t get_shifter_oper_with_rotate(uint32_t num, uint32_t rot) {
    num &= 0x0000000f;
    rot <<= 7;
    return (num | rot | 0x00000060);
}

uint32_t get_shifted_num(uint32_t num, uint32_t shift) {
    num >>= shift;
    num &= 0x000000ff;
    return num;
}


int parse_number(const char* expr, int* expr_ptr) {
    int result = 0;
    for (; expr[*expr_ptr] >= '0' && expr[*expr_ptr] <= '9'; (*expr_ptr)++) {
        result = result * 10 + expr[*expr_ptr] - '0';
    }

    return result;
}

int is_symbol(int symb) {
    return isalpha(symb) || symb == '_';
}

void parse_elem(const char* expr, int* expr_ptr, token_t* pre_poliz, int* ptr, const symbol_t* externs, int* flag, int* flag_bracket, int* bracket_counter) {
    for (; expr[*expr_ptr] == ' ';) {
        (*expr_ptr)++;
    }

    if (expr[*expr_ptr] == '\0') {
        pre_poliz[*ptr].type = END_OF_ARRAY;
        return;
    }
    
    char* name = calloc(256, sizeof(char));

    pre_poliz[*ptr].name = name;

    if (expr[*expr_ptr] == '+' ||
        expr[*expr_ptr] == '-' ||
        expr[*expr_ptr] == '*') {
            if ((*ptr) == 0 || (pre_poliz[(*ptr) - 1].type == OPERATION && pre_poliz[(*ptr) - 1].name[0] != ')')) {
                pre_poliz[*ptr].type = OPERATION;
                name[0] = '(';
                (*ptr)++;

                pre_poliz[*ptr].type = CONSTANT;
                pre_poliz[*ptr].num = 0;
                (*ptr)++;
                (*flag) += 1;

                flag_bracket[*flag] = (*bracket_counter);

                name = calloc(256, sizeof(char));
                pre_poliz[*ptr].name = name;
            }
            pre_poliz[*ptr].type = OPERATION;
        
            strcpy(name, "#");
            name[0] = expr[*expr_ptr];
            (*expr_ptr)++;

    } else if (expr[*expr_ptr] >= '0' && expr[*expr_ptr] <= '9') {
        pre_poliz[*ptr].type = CONSTANT;
        int num = parse_number(expr, expr_ptr);

        pre_poliz[*ptr].num = num;

        if ((*flag) > 0 && (*bracket_counter) == flag_bracket[*flag]) {
            (*ptr)++;

            pre_poliz[*ptr].type = OPERATION;
            name = calloc(256, sizeof(char));
            pre_poliz[*ptr].name = name;
            name[0] = ')';
            
            (*flag)--;
        }
        

    } else if (expr[*expr_ptr] == '(') {
        (*bracket_counter)++;
        pre_poliz[*ptr].type = OPERATION;
        strcpy(name, "(");
        (*expr_ptr)++;

    } else if (expr[*expr_ptr] == ')') {
        (*bracket_counter)--;
        pre_poliz[*ptr].type = OPERATION;
        strcpy(name, ")");
        (*expr_ptr)++;

        if ((*flag) > 0 && (*bracket_counter) == flag_bracket[*flag]) {
            (*ptr)++;

            pre_poliz[*ptr].type = OPERATION;
            name = calloc(256, sizeof(char));
            pre_poliz[*ptr].name = name;
            name[0] = ')';
            
            (*flag)--;
        }


    } else if (expr[*expr_ptr] == ',') {
        pre_poliz[*ptr].type = OPERATION;
        strcpy(name, ",");
        (*expr_ptr)++;

    } else {
        int i = 0;
        for (; is_symbol(expr[*expr_ptr]); (*expr_ptr)++) {
            name[i] = expr[*expr_ptr];
            i++;
        }
        name[i] = '\0';

        for (; expr[*expr_ptr] == ' ';) {
            (*expr_ptr)++;
        }

        for (int j = 0; externs[j].name != 0; j++) {
            if (strcmp(externs[j].name, name) == 0) {
                if (expr[*expr_ptr] == '(') {
                    pre_poliz[*ptr].type = FUNCTION;
                    pre_poliz[*ptr].func_ptr = externs[j].pointer;
                } else {
                    pre_poliz[*ptr].type = CONSTANT;
                    pre_poliz[*ptr].num = (*((int*) externs[j].pointer));

                    if ((*flag) > 0 && (*bracket_counter) == flag_bracket[*flag]) {
                        (*ptr)++;

                        pre_poliz[*ptr].type = OPERATION;
                        name = calloc(256, sizeof(char));
                        pre_poliz[*ptr].name = name;
                        name[0] = ')';
                        
                        (*flag)--;
                    }
                }
                break;
            }
        }
    }

    (*ptr)++;
}

int get_priority(token_t* token) {
    if (token->type == OPERATION) {
        if (token->name[0] == '(' || token->name[0] == ')') {
            return 0;
        } else if (token->name[0] == '+' || token->name[0] == '-') {
            return 1;
        } else if (token->name[0] == '*') {
            return 2;
        }
    } else if (token->type == FUNCTION) {
        return 3;
    }

    return -1;
}


void make_poliz_expr(const char* expr, token_t* poliz, token_t* pre_poliz, token_t* stack) {
    int stack_ptr = 0;
    int ptr = 0;
    int expr_ptr = 0;

    for (; pre_poliz[expr_ptr].type != END_OF_ARRAY; expr_ptr++) {
        if (pre_poliz[expr_ptr].type == CONSTANT) {
            poliz[ptr] = pre_poliz[expr_ptr];
            ptr++;

        } else if (pre_poliz[expr_ptr].type == FUNCTION) {
            stack[stack_ptr] = pre_poliz[expr_ptr];
            stack[stack_ptr].count_of_vars++;

            stack_ptr++;

        } else if (pre_poliz[expr_ptr].type == OPERATION) {
            if (pre_poliz[expr_ptr].name[0] == '(') {
                
                stack[stack_ptr] = pre_poliz[expr_ptr];
                stack_ptr++;
            } else if (pre_poliz[expr_ptr].name[0] == ')') {
                stack_ptr--;
                while (stack_ptr >= 0) {
                    if (stack[stack_ptr].name[0] == '(') {
                        break;
                    }

                    poliz[ptr] = stack[stack_ptr];
                    ptr++;
                    stack_ptr--;
                }
            } else if (pre_poliz[expr_ptr].name[0] == ',') {
                stack_ptr--;
                while (stack_ptr >= 0) {
                    if (stack[stack_ptr].type == OPERATION && stack[stack_ptr].name[0] == '(') {
                        stack[stack_ptr - 1].count_of_vars++;
                        break;
                    }
                    poliz[ptr] = stack[stack_ptr];
                    ptr++;
                    stack_ptr--;
                }
                stack_ptr++;

            } else {
                int my_prior = get_priority(&pre_poliz[expr_ptr]);
                stack_ptr--;
                while (stack_ptr >= 0) {
                    if (get_priority(&stack[stack_ptr]) < my_prior) {
                        break;
                    }
                    poliz[ptr] = stack[stack_ptr];
                    ptr++;
                    stack_ptr--;
                }
                stack_ptr++;

                stack[stack_ptr] = pre_poliz[expr_ptr];

                stack[stack_ptr].count_of_vars = 2;
                stack_ptr++;
            }

        }
    }

    stack_ptr--;
    while (stack_ptr >= 0) {
        poliz[ptr] = stack[stack_ptr];
        ptr++;
        stack_ptr--;
    }
    stack_ptr++;


    poliz[ptr].type = END_OF_ARRAY;
}


void parse_expression(const char* expr, token_t* poliz, int expr_len, const symbol_t* externs) {

    int ptr = 0;

    int expr_ptr = 0;

    
    token_t* pre_poliz = calloc(expr_len, sizeof(token_t));

    int flag = 0;
    int* flag_bracket = calloc(256, sizeof(int));
    int bracket_counter = 0;


    for (; expr[expr_ptr] != '\0';) {
        parse_elem(expr, &expr_ptr, pre_poliz, &ptr, externs, &flag, flag_bracket, &bracket_counter);
    }

    pre_poliz[ptr].type = END_OF_ARRAY;

    token_t* stack = calloc(expr_len, sizeof(token_t));
    
    make_poliz_expr(expr, poliz, pre_poliz, stack);

    free(pre_poliz);
    free(stack);
}

uint32_t* set_register_by_elem(uint32_t* out_buf, uint32_t reg, uint32_t num) {
    uint32_t mov_0 = ALL_COMMAND_PATTERN;
    mov_0 |= I_bit | MOVE_OPCODE | get_reg(reg, 1) | get_shifter_com(0u);

    (*out_buf) = mov_0;
    out_buf++;


    uint32_t add = ALL_COMMAND_PATTERN;
    add |= ADD_OPCODE | get_reg(reg, 1) | get_reg(reg, 0);

    uint32_t mov_r5 = ALL_COMMAND_PATTERN;
    mov_r5 |= I_bit | MOVE_OPCODE | get_reg(5u, 1);

    (*out_buf) = (mov_r5 | get_shifter_com(get_shifted_num(num, 0u)));
    out_buf++;

    (*out_buf) = (add | get_shifter_oper(5u));
    out_buf++;

    (*out_buf) = (mov_r5 | get_shifter_com(get_shifted_num(num, 8)));
    out_buf++;

    (*out_buf) = (add | get_shifter_oper_with_rotate(5u, 24));
    out_buf++;

    (*out_buf) = (mov_r5 | get_shifter_com(get_shifted_num(num, 16)));
    out_buf++;

    (*out_buf) = (add | get_shifter_oper_with_rotate(5u, 16));
    out_buf++;

    (*out_buf) = (mov_r5 | get_shifter_com(get_shifted_num(num, 24)));
    out_buf++;

    (*out_buf) = (add | get_shifter_oper_with_rotate(5u, 8));
    out_buf++;

    return out_buf;
}

uint32_t* binary_push_to_stack(token_t* token, uint32_t* out_buf) {

    uint32_t sub = ALL_COMMAND_PATTERN;
    sub |= I_bit | SUB_OPCODE | get_reg(13u, 1) | get_reg(13u, 0) | get_shifter_com(4u);
    
    uint32_t str = STR_PREFIX;
    str |= get_reg(0u, 1) | get_reg(13u, 0);

    
    (*out_buf) = sub;
    out_buf++;

    out_buf = set_register_by_elem(out_buf, 0u, (uint32_t)token->num);
    
    (*out_buf) = str;
    out_buf++;

    return out_buf;
}


uint32_t* binary_did_operation(token_t* token, uint32_t* out_buf) {
    uint32_t ldr = ALL_COMMAND_PATTERN;
    ldr |= LDR_PREFIX | get_reg(13u, 0);

    uint32_t add = ALL_COMMAND_PATTERN;
    add |= I_bit | ADD_OPCODE | get_reg(13u, 1) | get_reg(13u, 0) | get_shifter_com(4u);

    uint32_t sub = ALL_COMMAND_PATTERN;
    sub |= I_bit | SUB_OPCODE | get_reg(13u, 1) | get_reg(13u, 0) | get_shifter_com(4u);

    for (int i = token->count_of_vars - 1; i >= 0; i--) {
        (*out_buf) = (ldr | get_reg(i, 1));
        out_buf++;
        (*out_buf) = add;
        out_buf++;
    }



    uint32_t oper_to_do = ALL_COMMAND_PATTERN;

    if (token->name[0] == '+') {
        oper_to_do |= ADD_OPCODE | get_reg(0u, 1) | get_reg(0u, 0) | get_shifter_oper(1u);
        (*out_buf) = oper_to_do;
        out_buf++;

    } else if (token->name[0] == '-') {
        oper_to_do |= SUB_OPCODE | get_reg(0u, 1) | get_reg(0u, 0) | get_shifter_oper(1u); 
        (*out_buf) = oper_to_do;
        out_buf++;

    } else if (token->name[0] == '*') {
        oper_to_do |= REG_SHIFTER_OPER | get_shifter_oper(0u) | get_rs(1u) | get_reg(0u, 0);
        (*out_buf) = oper_to_do;
        out_buf++;

    } else {
        uint32_t mov_r4 = ALL_COMMAND_PATTERN;
        mov_r4 |= I_bit | MOVE_OPCODE | get_reg(4u, 1) | get_shifter_com(0u);

        (*out_buf) = mov_r4;
        out_buf++;

        uint32_t func_ptr = (uint32_t)token->func_ptr;

        out_buf = set_register_by_elem(out_buf, 4u, func_ptr);

        uint32_t mov_lr = ALL_COMMAND_PATTERN;
        mov_lr |= MOVE_OPCODE | get_reg(14u, 1) | get_shifter_com(15u);

        (*out_buf) = mov_lr;
        out_buf++;

        uint32_t add_lr = ALL_COMMAND_PATTERN;
        add_lr |= I_bit | ADD_OPCODE | get_reg(14u, 1) | get_reg(14u, 0) | get_shifter_com(4u);

        (*out_buf) = add_lr;
        out_buf++;

        (*out_buf) = BX_R4;
        out_buf++;
    }

    (*out_buf) = sub;
    out_buf++;

    uint32_t str = STR_PREFIX;
    str |= get_reg(0u, 1) | get_reg(13u, 0);

    (*out_buf) = str;
    out_buf++;

    return out_buf;
}

void make_binary(token_t* poliz, uint32_t* out_buf) {

    (*out_buf) = PUSH_lr;
    out_buf++;

    
    for (int indx = 0; poliz[indx].type != END_OF_ARRAY; indx++) {
        if (poliz[indx].type == CONSTANT) {
            out_buf = binary_push_to_stack(&poliz[indx], out_buf);
        } else {
            out_buf = binary_did_operation(&poliz[indx], out_buf);
        }
    }
    
    uint32_t ldr = ALL_COMMAND_PATTERN;
    ldr |= LDR_PREFIX | get_reg(13u, 0) | get_reg(0, 1);
    (*out_buf) = ldr;
    out_buf++;
    

    uint32_t add = ALL_COMMAND_PATTERN;
    add |= I_bit | ADD_OPCODE | get_reg(13u, 1) | get_reg(13u, 0) | get_shifter_com(4u);

    (*out_buf) = add;
    out_buf++;

    (*out_buf) = POP_lr;
    out_buf++;

    (*out_buf) = BX_lr;
    out_buf++;
}


void jit_compile_expression_to_arm(const char * expression,
                               const symbol_t * externs,
                               void * out_buffer) {
    size_t expression_len = strlen(expression) + 100;
    token_t* poliz = calloc(expression_len, sizeof(token_t));

    parse_expression(expression, poliz, expression_len, externs);

    make_binary(poliz, out_buffer);
}