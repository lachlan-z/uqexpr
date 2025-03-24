#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <tinyexpr.h>

typedef struct {
    char* name;
    double value;
} Var;

typedef struct {
    char* name;
    double value;
    double start;
    double increment;
    double end;
} Loop;

void variable_check_null(char* variable)
{
    if (variable == NULL) {
        fprintf(stderr,
                "Usage: ./uqexpr [--forloop string] [--define string] "
                "[--significantfigs 2..8] [inputfilename]\n");
        exit(11);
    }
}

void variable_check_name(char* variable_name)
{
    if (!((size_t)1 <= strlen(variable_name)
                && strlen(variable_name) <= (size_t)25)) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    for (size_t l = 0; l < strlen(variable_name); l++) {
        if (isalpha(variable_name[l]) == 0) {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(12);
        }
    }
}

void variable_check_name_op(char* variable_name)
{
    if (!((size_t)1 <= strlen(variable_name)
                && strlen(variable_name) <= (size_t)25)) {
        fprintf(stderr, "Invalid command, expression or assignment operation\n");
    }

    for (size_t l = 0; l < strlen(variable_name); l++) {
        if (isalpha(variable_name[l]) == 0) {
            fprintf(stderr, "Invalid command, expression or assignment operation\n");
        }
    }
}

void value_check(char* value)
{
    int decimal_count = 0;
    int negative_num = 0;

    if (value[0] == '-') {
        negative_num = 1;
    }
    for (size_t i = negative_num; i < strlen(value); i++) {
        if (value[i] == '.') {
            decimal_count++;
        }
        if (isdigit(value[i]) == 0 && value[i] != '.') {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(12);
        }
    }
}

void variable_check_define(char* variable)
{
    char* string_parse = strtok(strdup(variable), "=");
    char* value = strtok(NULL, "=");

    int equals_count = 0;
    for (size_t i = 0; i < strlen(variable); i++) {
        if (variable[i] == '=') {
            equals_count++;
        }
        if (equals_count > 1) {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(12);
        }
    }

    if (string_parse == NULL || value == NULL) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    value_check(value);
    variable_check_name(string_parse);
    free(string_parse);
}

void variable_check_loop(char* variable)
{
    char* string_parse = strtok(strdup(variable), ",");
    variable_check_name(string_parse);

    char* start = strtok(NULL, ",");
    char* increment = strtok(NULL, ",");
    char* end = strtok(NULL, ",");
    if (string_parse == NULL || start == NULL || increment == NULL
            || end == NULL) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    value_check(start);
    value_check(increment);
    value_check(end);

    double start_d = atof(start);
    double increment_d = atof(increment);
    double end_d = atof(end);

    if ((increment_d == 0) || ((start_d < end_d) && (increment_d < 0))
            || ((start_d > end_d) && (increment_d > 0))) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }
}

void variable_check_sigfig(char* variable)
{
    if ((variable[0] == '0') || (atof(variable) < 2) || (atof(variable) > 8)) {
        fprintf(stderr,
                "Usage: ./uqexpr [--forloop string] [--define string] "
                "[--significantfigs 2..8] [inputfilename]\n");
        exit(11);
    }
}

void free_variables(Var* variables, int variables_count)
{
    for (int i = 0; i < variables_count; i++) {
        free(variables[i].name);
    }
    free(variables);
}

void free_loops(Loop* loops, int loops_count)
{
    for (int i = 0; i < loops_count; i++) {
        free(loops[i].name);
    }
    free(loops);
}

void command_arg_check(int argc, char** argv, Var** variables, Loop** loops,
        int* variables_count, int* loops_count, int* significant_figs,
        FILE** file)
{
    for (int i = 1; i < argc; i++) {
        // printf("%d: %s\n", i, argv[i]);
        // printf("argc: %d\n", argc);
        if (strcmp(argv[i], "--define") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_define(argv[i + 1]);
            char* string_parse = strtok(argv[i + 1], "=");

            *variables
                    = realloc(*variables, sizeof(Var) * (*variables_count + 1));

            (*variables)[*variables_count].name
                    = malloc(strlen(string_parse) + 1);
            strcpy((*variables)[*variables_count].name, string_parse);

            (*variables)[*variables_count].value = atof(strtok(NULL, "="));

            (*variables_count)++;

        } else if (strcmp(argv[i], "--forloop") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_loop(argv[i + 1]);
            char* string_parse = strtok(argv[i + 1], ",");

            *loops = realloc(*loops, sizeof(Loop) * (*loops_count + 1));

            (*loops)[*loops_count].name = malloc(strlen(string_parse) + 1);
            strcpy((*loops)[*loops_count].name, string_parse);

            double value_start = atof(strtok(NULL, ","));

            (*loops)[*loops_count].value = value_start;
            (*loops)[*loops_count].start = value_start;
            (*loops)[*loops_count].increment = atof(strtok(NULL, ","));
            (*loops)[*loops_count].end = atof(strtok(NULL, ","));

            (*loops_count)++;

        } else if (strcmp(argv[i], "--significantfigs") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_sigfig(argv[i + 1]);

            (*significant_figs) = atoi(argv[i + 1]) + 1;
        } else if ((i == argc - 1) && (argv[i][0] != '-')
                && (argv[i - 1][0] != '-') && (strcmp(argv[i], "") != 0)) {
            (*file) = fopen(argv[i], "r");
            if ((*file) == NULL) {
                free_variables(*variables, *variables_count);
                free_loops(*loops, *loops_count);
                fprintf(stderr,
                        "uqexpr: unable to open file \"%s\" for reading\n",
                        argv[i]);
                exit(4);
            }
        } else if (argv[i][0] == '-' && argv[i][1] == '-') {
            free_variables(*variables, *variables_count);
            free_loops(*loops, *loops_count);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(11);
        } else if (strcmp(argv[i], "") == 0
                || ((i != argc - 1) && (argv[i][0] == '/'))) {
            free_variables(*variables, *variables_count);
            free_loops(*loops, *loops_count);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(11);
        }
    }
    // ee
    for (int i = 0; i < *loops_count; i++) {
        for (int l = 0; l < *loops_count; l++) {
            if ((i != l) && (strcmp((*loops)[i].name, (*loops)[l].name) == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(5);
            }
        }
    }

    for (int i = 0; i < *variables_count; i++) {
        for (int l = 0; l < *variables_count; l++) {
            if ((i != l)
                    && (strcmp((*variables)[i].name, (*variables)[l].name)
                            == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(5);
            }
        }
    }
}

#define INITIAL_BUFFER_SIZE 80

char* read_line(FILE* stream)
{

    int bufferSize = INITIAL_BUFFER_SIZE;
    char* buffer = malloc(sizeof(char) * bufferSize);
    int numRead = 0;
    int next;

    if (feof(stream)) {
        return NULL;
    }

    while (1) {
        next = fgetc(stream);

        if (next == EOF && numRead == 0) {
            free(buffer);
            return NULL;
        }

        if (numRead == bufferSize - 1) {
            bufferSize *= 2;
            buffer = realloc(buffer, sizeof(char) * bufferSize);
        }

        if (next == '\n' || next == EOF) {
            buffer[numRead] = '\0';
            break;
        }

        buffer[numRead++] = next;
    }
    return buffer;
}

/**
 * Separates segments of string separated by delim into array of char*
 **/
char** separate_line(char* string, char delim)
{
    char** tokens = malloc(sizeof(char*));
    tokens[0] = NULL;

    char delimiters[] = {delim, '\0'};

    int numTokens = 0;
    char* token = strtok(string, delimiters);

    while (token) {
        tokens[numTokens] = strdup(token);
        numTokens++;
        tokens = realloc(tokens, sizeof(char*) * (numTokens + 1));
        tokens[numTokens] = NULL;

        token = strtok(NULL, delimiters);
    }

    return tokens;
}

te_variable* struct_to_te_var(Var** variables, Loop** loops, int* variables_count, int* loops_count) {
    int var_count_checked;
    if (variables_count != NULL) {
        var_count_checked = (*variables_count);
    } else {
        var_count_checked = 0;
    }

    int loop_count_checked;
    if (loops_count != NULL) {
        loop_count_checked = (*loops_count);
    } else {
        loop_count_checked = 0;
    }

    int total = var_count_checked + loop_count_checked;
    te_variable* result = (te_variable*)malloc(sizeof(te_variable) * (total));
    for (int i = 0; i < var_count_checked; i++) {
        result[i].name = (*variables)[i].name;
        result[i].address = &((*variables)[i].value);
    }

    for (int i = 0; i < loop_count_checked; i++) {
        result[i + total].name = (*loops)[i].name;
        result[i + total].address = &((*loops)[i].value);
    }

    return result;
    
}

void main_handler(FILE* file, int* significant_figs, Var** variables, int* variables_count, Loop** loops, int* loops_count)
{
    while (1) {
        char* line = read_line(file);
        // printf("\nstdin: %s\n", line);

        if (feof(file)) {
            fprintf(stdout, "Thank you for using uqexpr.\n");
            exit(0);
        }
        if (line[0] == '#') {
            continue;
        } else {
            char** separated_line = separate_line(line, '=');

            if (separated_line[1] == NULL) {
                int error_compile = 0;
 
                te_variable* vars = struct_to_te_var(variables, loops, variables_count, loops_count);
                te_expr* expr = te_compile(line, vars, (*variables_count)+(*loops_count), &error_compile);
                double result = te_eval(expr);
                if (error_compile == 0) {
                    fprintf(stdout, "Result = %.*g\n", *significant_figs,
                            result);
                } else {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                }
            } else {
                char* lhs = separated_line[0];
                char* rhs = separated_line[1];
                int error_compile = 0;

                variable_check_name_op(lhs);
                
                te_variable* vars = struct_to_te_var(variables, loops, variables_count, loops_count);
                te_expr* expr = te_compile(rhs, vars, (*variables_count)+(*loops_count), &error_compile);
                
                if (error_compile != 0) {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                } else {
                    *variables
                        = realloc(*variables, sizeof(Var) * (*variables_count + 1));

                    (*variables)[*variables_count].name
                        = malloc(strlen(lhs) + 1);
                    strcpy((*variables)[*variables_count].name, lhs);

                    double result = te_eval(expr);

                    (*variables)[*variables_count].value = result;

                    (*variables_count)++;

                    fprintf(stdout, "%s = %.*g\n", lhs, *significant_figs,
                            result);

                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    Var* variables = NULL;
    Loop* loops = NULL;
    FILE* file = NULL;

    int variables_count = 0;
    int loops_count = 0;
    int significant_figs = 4;

    command_arg_check(argc, argv, &variables, &loops, &variables_count,
            &loops_count, &significant_figs, &file);

    fprintf(stdout,
            "Welcome to uqexpr.\nThis program was written by s4808239.\n");

    if (variables == NULL) {
        fprintf(stdout, "There are no variables.\n");
    } else {
        fprintf(stdout, "Variables:\n");
        for (int i = 0; i < variables_count; i++) {
            fprintf(stdout, "%s = %.*g\n", variables[i].name, significant_figs,
                    variables[i].value);
        }
    }

    if (loops == NULL) {
        fprintf(stdout, "There are no loop variables.\n");
    } else {
        fprintf(stdout, "Loop variables:\n");
        for (int i = 0; i < loops_count; i++) {
            fprintf(stdout, "%s = %.*g (%.*g, %.*g, %.*g)\n", loops[i].name,
                    significant_figs, loops[i].start, significant_figs,
                    loops[i].start, significant_figs, loops[i].increment,
                    significant_figs, loops[i].end);
        }
    }

    if (file == NULL) {
        fprintf(stdout,
                "Please enter your expressions and assignment operations to be "
                "evaluated.\n");
        main_handler(stdin, &significant_figs, &variables, &variables_count, &loops, &loops_count);
    } else {
        main_handler(file, &significant_figs, &variables, &variables_count, &loops, &loops_count);
    }

    return 0;
}
