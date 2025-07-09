/* ============================================================================
 * LISP Interprtor
 * Developed on Mac Silicon ARM machine using Apple Clang Compiler as target.
 * C library documentation used: https://devdocs.io/c/
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

// Stack Data Structure
typedef struct node {
    int32_t data;
    struct node *next;
} node_t;

typedef struct Stack {
    node_t *head;
} Stack;

static int32_t stack_is_empty(Stack *stack) {
    return stack->head == NULL;
}

static int32_t stack_init(Stack *stack) {
    stack->head = NULL;
    return 0;
}

static int32_t stack_insert(Stack *stack, int32_t value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        return -1;
    }
    new_node->data = value;
    new_node->next = stack->head;
    stack->head = new_node;
    return 0;
}

static int32_t stack_pop(Stack *stack) {
    if (stack_is_empty(stack)) {
        return -1;
    }
    node_t *temp_node = stack->head;
    int32_t popped_value = temp_node->data;
    stack->head = stack->head->next;
    free(temp_node);
    return popped_value;
}

static void clear_stack(Stack *stack) {
    while (stack->head != NULL) {
        node_t *current_node = stack->head;
        stack->head = stack->head->next;
        free(current_node);
    }
}

// PSI Value System
typedef enum {
    PVAL_NUMBER,
    PVAL_BOOL,
    PVAL_SYMBOL,
    PVAL_LIST,
    PVAL_FUNCTION,
    PVAL_ERROR
} pval_t;

struct pval;
typedef struct pval *(*builtin_function_ptr)(struct pval **args, int32_t arg_count);

typedef struct pval {
    pval_t type;
    double number;
    bool boolean;
    char *symbol;
    struct pval **list_items;
    int32_t list_count;
    int32_t list_capacity;
    builtin_function_ptr function;
    char *error_type;
    char *error_message;
} pval;

// PSI Constructors
static pval *pval_number(double number_val);
static pval *pval_bool(bool bool_val);
static pval *pval_symbol(const char *symbol_str);
static pval *pval_function(builtin_function_ptr func);
static pval *pval_list(void);
static pval *pval_error(const char *error_type, const char *error_message);
static void pval_delete(pval *target_value);
static void pval_print(pval *target_value);
static void pval_add(pval *target_list, pval *new_item);
static pval *pval_parse(char **input_ptr);
static pval *pval_eval(pval *input_value);

pval *pval_number(double number_val) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_NUMBER,
        .number = number_val
    };
    return new_value;
}

pval *pval_bool(bool bool_val) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_BOOL,
        .boolean = bool_val
    };
    return new_value;
}

pval *pval_symbol(const char *symbol_str) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    char *symbol_copy = strdup(symbol_str);
    if (symbol_copy == NULL) {
        free(new_value);
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_SYMBOL,
        .symbol = symbol_copy
    };
    return new_value;
}

pval *pval_function(builtin_function_ptr func_ptr) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_FUNCTION,
        .function = func_ptr
    };
    return new_value;
}

pval *pval_list(void) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    pval **initial_items = malloc(4 * sizeof(pval *));
    if (initial_items == NULL) {
        free(new_value);
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_LIST,
        .list_items = initial_items,
        .list_count = 0,
        .list_capacity = 4
    };
    return new_value;
}

pval *pval_error(const char *error_type, const char *error_message) {
    pval *new_value = malloc(sizeof(pval));
    if (new_value == NULL) {
        return NULL;
    }
    char *type_copy = strdup(error_type);
    char *message_copy = strdup(error_message);
    if (type_copy == NULL || message_copy == NULL) {
        free(type_copy);
        free(message_copy);
        free(new_value);
        return NULL;
    }
    *new_value = (pval){
        .type = PVAL_ERROR,
        .error_type = type_copy,
        .error_message = message_copy
    };
    return new_value;
}

// P Value Handling Functions
void pval_delete(pval *target_value) {
    if (target_value == NULL) {
        return;
    }
    switch (target_value->type) {
    case PVAL_SYMBOL:
        free(target_value->symbol);
        break;
    case PVAL_LIST:
        for (int32_t i = 0; i < target_value->list_count; i++) {
            pval_delete(target_value->list_items[i]);
        }
        free(target_value->list_items);
        break;
    case PVAL_ERROR:
        free(target_value->error_type);
        free(target_value->error_message);
        break;
    case PVAL_NUMBER:
    case PVAL_BOOL:
    case PVAL_FUNCTION:
        break;
    }
    free(target_value);
}

void pval_print(pval *target_value) {
    if (target_value == NULL) {
        printf("NULL_PVAL");
        return;
    }
    switch (target_value->type) {
    case PVAL_NUMBER:
        if (target_value->number == (int32_t)target_value->number) {
            printf("%d", (int32_t)target_value->number);
        } else {
            printf("%.3f", target_value->number);
        }
        break;
    case PVAL_BOOL:
        printf(target_value->boolean ? "#t" : "#f");
        break;
    case PVAL_SYMBOL:
        printf("%s", target_value->symbol);
        break;
    case PVAL_LIST:
        printf("(");
        for (int32_t i = 0; i < target_value->list_count; i++) {
            pval_print(target_value->list_items[i]);
            if (i < target_value->list_count - 1) {
                printf(" ");
            }
        }
        printf(")");
        break;
    case PVAL_ERROR:
        printf("$error{%s %s}", target_value->error_type, target_value->error_message);
        break;
    case PVAL_FUNCTION:
        printf("<function>");
        break;
    }
}


void pval_add(pval *target_list, pval *new_item) {
    if (target_list == NULL || target_list->type != PVAL_LIST || new_item == NULL) {
        return;
    }
    if (target_list->list_count < 0 || target_list->list_capacity <= 0) {
        return; // Invalid list state
    }
    if (target_list->list_count >= target_list->list_capacity) {
        size_t new_capacity;
        if (target_list->list_capacity > SIZE_MAX / 2) {
            return; // Would overflow
        }
        new_capacity = target_list->list_capacity * 2;
        pval **expanded_items = realloc(target_list->list_items,
                                      new_capacity * sizeof(pval *));
        if (expanded_items == NULL) {
            return;
        }
        target_list->list_items = expanded_items;
        target_list->list_capacity = new_capacity;
    }
    if (target_list->list_count < target_list->list_capacity) {
        target_list->list_items[target_list->list_count++] = new_item;
    }
}

// Interpretor Parser
static void skip_whitespace(char **input_ptr) {
    while (isspace(**input_ptr)) {
        (*input_ptr)++;
    }
}

pval *pval_parse(char **input_ptr) {
    skip_whitespace(input_ptr);
    if (**input_ptr == '\0') {
        return NULL;
    }

    if (**input_ptr == '(') {
        (*input_ptr)++;
        pval *list_value = pval_list();
        if (list_value == NULL) {
            return pval_error("MemoryError", "Failed to allocate list");
        }

        while (true) {
            skip_whitespace(input_ptr);
            if (**input_ptr == '\0') {
                pval_delete(list_value);
                return pval_error("SyntaxError", "Unexpected EOF, expected ')'");
            }
            if (**input_ptr == ')') {
                (*input_ptr)++;
                break;
            }
            pval *parsed_item = pval_parse(input_ptr);
            if (parsed_item == NULL) {
                pval_delete(list_value);
                return pval_error("SyntaxError", "Invalid expression inside list");
            }
            if (parsed_item->type == PVAL_ERROR) {
                pval_delete(list_value);
                return parsed_item;
            }
            pval_add(list_value, parsed_item);
        }
        return list_value;
    } else if (isdigit(**input_ptr) || (**input_ptr == '-' && isdigit((*input_ptr)[1]))
               || **input_ptr == '.') {
        char *end_ptr;
        double parsed_num = strtod(*input_ptr, &end_ptr);
        if (end_ptr == *input_ptr) {
            return pval_error("SyntaxError", "Invalid number format");
        }
        *input_ptr = end_ptr;
        return pval_number(parsed_num);
    } else if (strncmp(*input_ptr, "#t", 2) == 0) {
        *input_ptr += 2;
        return pval_bool(true);
    } else if (strncmp(*input_ptr, "#f", 2) == 0) {
        *input_ptr += 2;
        return pval_bool(false);
    } else {
        char symbol_buffer[256];
        int32_t i = 0;
        while (**input_ptr != '\0' && !isspace(**input_ptr) && **input_ptr != '('
               && **input_ptr != ')') {
            if (i >= sizeof(symbol_buffer) - 1) {
                return pval_error("SyntaxError", "Symbol too long");
            }
            symbol_buffer[i++] = **input_ptr;
            (*input_ptr)++;
        }
        if (i == 0) {
            return pval_error("SyntaxError", "Empty symbol or unparsable token");
        }
        symbol_buffer[i] = '\0';
        return pval_symbol(symbol_buffer);
    }
}

// Builtin PSI Op Functions
pval *builtin_add(pval **args, int32_t arg_count);
pval *builtin_sub(pval **args, int32_t arg_count);
pval *builtin_mul(pval **args, int32_t arg_count);
pval *builtin_div(pval **args, int32_t arg_count);
pval *builtin_eq(pval **args, int32_t arg_count);
pval *builtin_quit(pval **args, int32_t arg_count);

pval *builtin_add(pval **args, int32_t arg_count) {
    double running_sum = 0.0;
    if (arg_count == 0) {
        return pval_number(0.0);
    }

    for (int32_t i = 0; i < arg_count; i++) {
        if (args[i]->type != PVAL_NUMBER) {
            return pval_error("TypeError", "Arguments to + must be numbers");
        }
        running_sum += args[i]->number;
    }
    return pval_number(running_sum);
}

pval *builtin_sub(pval **args, int32_t arg_count) {
    if (arg_count == 0) {
        return pval_error("ArityError", "'-' requires at least one argument");
    }
    if (args[0]->type != PVAL_NUMBER) {
        return pval_error("TypeError", "First argument to - must be a number");
    }

    if (arg_count == 1) {
        return pval_number(-args[0]->number);
    } else if (arg_count == 2) {
        if (args[1]->type != PVAL_NUMBER) {
            return pval_error("TypeError", "Second argument to - must be a number");
        }
        return pval_number(args[0]->number - args[1]->number);
    }
    return pval_error("ArityError", "'-' currently supports 1 or 2 arguments");
}

pval *builtin_mul(pval **args, int32_t arg_count) {
    double running_product = 1.0;
    if (arg_count == 0) {
        return pval_number(1.0);
    }

    for (int32_t i = 0; i < arg_count; i++) {
        if (args[i]->type != PVAL_NUMBER) {
            return pval_error("TypeError", "Arguments to * must be numbers");
        }
        running_product *= args[i]->number;
    }
    return pval_number(running_product);
}

pval *builtin_div(pval **args, int32_t arg_count) {
    if (arg_count != 2) {
        return pval_error("ArityError", "'/' requires exactly 2 arguments");
    }
    if (args[0]->type != PVAL_NUMBER || args[1]->type != PVAL_NUMBER) {
        return pval_error("TypeError", "Arguments to / must be numbers");
    }
    if (args[1]->number == 0.0) {
        return pval_error("DivisionByZeroError", "Division by zero");
    }
    return pval_number(args[0]->number / args[1]->number);
}

pval *builtin_eq(pval **args, int32_t arg_count) {
    if (arg_count != 2) {
        return pval_error("ArityError", "'=' requires exactly 2 arguments");
    }

    pval *first_arg = args[0];
    pval *second_arg = args[1];

    if (first_arg->type != second_arg->type) {
        return pval_bool(false);
    }

    switch (first_arg->type) {
    case PVAL_NUMBER:
        return pval_bool(fabs(first_arg->number - second_arg->number) < 1e-10);
    case PVAL_BOOL:
        return pval_bool(first_arg->boolean == second_arg->boolean);
    case PVAL_SYMBOL:
        return pval_bool(strcmp(first_arg->symbol, second_arg->symbol) == 0);
    default:
        return pval_error("TypeError", "Unsupported types for equality comparison");
    }
}

pval *builtin_quit(pval **args, int32_t arg_count) {
    if (arg_count != 0) {
        return pval_error("ArityError", "quit takes no arguments");
    }
    return pval_symbol("quitting");
}

// Builtin Op Function Struct
typedef struct builtin {
    const char *name;
    builtin_function_ptr func;
} builtin_t;

builtin_t builtins[] = {
    {"+", builtin_add},
    {"-", builtin_sub},
    {"*", builtin_mul},
    {"/", builtin_div},
    {"=", builtin_eq},
    {"quit", builtin_quit},
    {NULL, NULL}
};

pval *pval_eval(pval *input_value) {
    if (input_value == NULL) {
        return NULL;
    }

    if (input_value->type == PVAL_NUMBER || input_value->type == PVAL_BOOL
        || input_value->type == PVAL_ERROR) {
        if (input_value->type == PVAL_NUMBER) {
            return pval_number(input_value->number);
        }
        if (input_value->type == PVAL_BOOL) {
            return pval_bool(input_value->boolean);
        }
        return pval_error(input_value->error_type, input_value->error_message);
    }

    if (input_value->type == PVAL_SYMBOL) {
        for (int32_t i = 0; builtins[i].name != NULL; i++) {
            if (strcmp(input_value->symbol, builtins[i].name) == 0) {
                return pval_function(builtins[i].func);
            }
        }
        return pval_error("UnboundError", "Symbol not bound to a function");
    }

    if (input_value->type == PVAL_LIST) {
        if (input_value->list_count < 0) {
            return pval_error("ListError", "Invalid list count");
        }
        
        if (input_value->list_count == 0) {
            return pval_list();
        }

        pval **evaluated_items = malloc(input_value->list_count * sizeof(pval *));
        if (evaluated_items == NULL) {
            return pval_error("MemoryError", "Failed to allocate evaluated items");
        }

        for (int32_t i = 0; i < input_value->list_count; i++) {
            if (i >= input_value->list_count) {
                for (int32_t k = 0; k < i; k++) {
                    pval_delete(evaluated_items[k]);
                }
                free(evaluated_items);
                return pval_error("ListError", "List index out of bounds");
            }
            evaluated_items[i] = pval_eval(input_value->list_items[i]);
            if (evaluated_items[i] == NULL || evaluated_items[i]->type == PVAL_ERROR) {
                pval *error_result = evaluated_items[i]
                    ? pval_error(evaluated_items[i]->error_type,
                                 evaluated_items[i]->error_message)
                    : pval_error("EvalError", "Null evaluation result");
                for (int32_t k = 0; k <= i; k++) {
                    pval_delete(evaluated_items[k]);
                }
                free(evaluated_items);
                return error_result;
            }
        }

        pval *function_head = evaluated_items[0];
        if (function_head->type != PVAL_FUNCTION) {
            pval *error_result = pval_error("InapplicableHeadError",
                                            "Expression head is not a function");
            for (int32_t i = 0; i < input_value->list_count; i++) {
                pval_delete(evaluated_items[i]);
            }
            free(evaluated_items);
            return error_result;
        }

        pval **function_args = NULL;
        int32_t num_args = input_value->list_count - 1;
        if (num_args > 0) {
            function_args = malloc(num_args * sizeof(pval *));
            if (function_args == NULL) {
                for (int32_t i = 0; i < input_value->list_count; i++) {
                    pval_delete(evaluated_items[i]);
                }
                free(evaluated_items);
                return pval_error("MemoryError",
                                  "Failed to allocate arguments for function call");
            }
            for (int32_t i = 0; i < num_args; i++) {
                if (i+1 >= input_value->list_count) {
                    for (int32_t k = 0; k < input_value->list_count; k++) {
                        pval_delete(evaluated_items[k]);
                    }
                    free(evaluated_items);
                    free(function_args);
                    return pval_error("ListError", "List index out of bounds");
                }
                function_args[i] = evaluated_items[i + 1];
            }
        }

        pval *eval_result = function_head->function(function_args, num_args);

        pval_delete(function_head);
        free(evaluated_items);
        free(function_args);

        return eval_result;
    }

    return pval_error("EvalError", "Unsupported pval type for evaluation");
}

static bool check_balanced_parens(char *input_str, Stack *paren_stack) {
    clear_stack(paren_stack);
    for (char *i = input_str; *i != '\0'; i++) {
        if (*i == '(') {
            stack_insert(paren_stack, '(');
        } else if (*i == ')') {
            if (stack_is_empty(paren_stack)) {
                clear_stack(paren_stack);
                return false;
            }
            stack_pop(paren_stack);
        }
    }
    bool balanced = stack_is_empty(paren_stack);
    clear_stack(paren_stack);
    return balanced;
}

int32_t main(void) {
    Stack paren_stack;
    stack_init(&paren_stack);

    char input_buffer[1024] = {0};

    while (true) {
        printf("psi> ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nQuitting...\n");
                break;
            } else if (ferror(stdin)) {
                printf("$error{IOError Input error}\n");
                clearerr(stdin);
                continue;
            }
        }

        if (strlen(input_buffer) == sizeof(input_buffer) - 1 && input_buffer[sizeof(input_buffer) - 2] != '\n') {
            printf("$error{InputError Input exceeds maximum size of %zu bytes}\n", sizeof(input_buffer) - 1);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            memset(input_buffer, 0, sizeof(input_buffer));
            continue;
        }

        char *newline = strchr(input_buffer, '\n');
        if (newline) *newline = '\0';

        if (input_buffer[0] == '\0') {
            printf("$error{SyntaxError Empty input}\n");
            continue;
        }

        if (!check_balanced_parens(input_buffer, &paren_stack)) {
            printf("$error{SyntaxError Unbalanced parentheses}\n");
            continue;
        }

        char *parse_ptr = input_buffer;
        pval *parsed_value = pval_parse(&parse_ptr);

        if (parsed_value == NULL) {
            printf("$error{SyntaxError Empty input or unparsable}\n");
            continue;
        }
        if (parsed_value->type == PVAL_ERROR) {
            pval_print(parsed_value);
            printf("\n");
            pval_delete(parsed_value);
            continue;
        }

        if (parsed_value->type == PVAL_LIST && 
            parsed_value->list_count == 1 &&
            parsed_value->list_items[0]->type == PVAL_SYMBOL &&
            strcmp(parsed_value->list_items[0]->symbol, "quit") == 0) {
            printf("Quitting...\n");
            pval_delete(parsed_value);
            break;
        }

        pval *final_result = pval_eval(parsed_value);
        pval_delete(parsed_value);

        if (final_result != NULL) {
            if (final_result->type == PVAL_SYMBOL && 
                strcmp(final_result->symbol, "quitting") == 0) {
                printf("Quitting...\n");
                pval_delete(final_result);
                break;
            }
            
            pval_print(final_result);
            printf("\n");
            pval_delete(final_result);
        } else {
            printf("$error{EvalError Null result from evaluation}\n");
        }

        memset(input_buffer, 0, sizeof(input_buffer));
    }

    return 0;
}