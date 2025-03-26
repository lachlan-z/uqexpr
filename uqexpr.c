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

void variable_check_name(char* variableName)
{
    if (!((size_t)1 <= strlen(variableName)
                && strlen(variableName) <= (size_t)25)) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    for (size_t l = 0; l < strlen(variableName); l++) {
        if (isalpha(variableName[l]) == 0) {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(12);
        }
    }
}

int variable_check_name_op(char* variableName)
{
    if (!((size_t)1 <= strlen(variableName)
                && strlen(variableName) <= (size_t)25)) {
        fprintf(stderr,
                "Invalid command, expression or assignment operation\n");
        return 1;
    }

    for (size_t l = 0; l < strlen(variableName); l++) {
        if (isalpha(variableName[l]) == 0) {
            fprintf(stderr,
                    "Invalid command, expression or assignment operation\n");
            return 1;
        }
    }
    return 0;
}

void value_check(char* value)
{
    int decimalCount = 0;
    int negativeNum = 0;

    if (value[0] == '-') {
        negativeNum = 1;
    }
    for (size_t i = negativeNum; i < strlen(value); i++) {
        if (value[i] == '.') {
            decimalCount++;
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
    char* stringParse = strtok(strdup(variable), "=");
    char* value = strtok(NULL, "=");

    int equalCount = 0;
    for (size_t i = 0; i < strlen(variable); i++) {
        if (variable[i] == '=') {
            equalCount++;
        }
        if (equalCount > 1) {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(12);
        }
    }

    if (stringParse == NULL || value == NULL) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    value_check(value);
    variable_check_name(stringParse);
    free(stringParse);
}

void variable_check_loop(char* variable)
{
    char* stringParse = strtok(strdup(variable), ",");
    variable_check_name(stringParse);

    char* start = strtok(NULL, ",");
    char* increment = strtok(NULL, ",");
    char* end = strtok(NULL, ",");
    if (stringParse == NULL || start == NULL || increment == NULL
            || end == NULL) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    value_check(start);
    value_check(increment);
    value_check(end);

    double startD = atof(start);
    double incrementD = atof(increment);
    double endD = atof(end);

    if ((incrementD == 0) || ((startD < endD) && (incrementD < 0))
            || ((startD > endD) && (incrementD > 0))) {
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

int loop_check_op(char* variable)
{
    if (variable == NULL) {
        fprintf(stderr,
                "Invalid command, expression or assignment operation\n");
        return 1;
    }

    char* stringParse = strtok(strdup(variable), ",");
    variable_check_name(stringParse);

    char* start = strtok(NULL, ",");
    char* increment = strtok(NULL, ",");
    char* end = strtok(NULL, ",");
    if (stringParse == NULL || start == NULL || increment == NULL
            || end == NULL) {
        fprintf(stderr,
                "Invalid command, expression or assignment operation\n");
        return 1;
    }

    value_check(start);
    value_check(increment);
    value_check(end);

    double startD = atof(start);
    double incrementD = atof(increment);
    double endD = atof(end);

    if ((incrementD == 0) || ((startD < endD) && (incrementD < 0))
            || ((startD > endD) && (incrementD > 0))) {
        fprintf(stderr,
                "Invalid command, expression or assignment operation\n");
        return 1;
    }
    return 0;
}

void free_variables(Var* variables, int variablesCount)
{
    for (int i = 0; i < variablesCount; i++) {
        free(variables[i].name);
    }
    free(variables);
}

void free_loops(Loop* loops, int loopsCount)
{
    for (int i = 0; i < loopsCount; i++) {
        free(loops[i].name);
    }
    free(loops);
}

void command_arg_check(int argc, char** argv, Var** variables, Loop** loops,
        int* variablesCount, int* loopsCount, int* significantFigs,
        FILE** file)
{
    for (int i = 1; i < argc; i++) {
        // printf("%d: %s\n", i, argv[i]);
        // printf("argc: %d\n", argc);
        if (strcmp(argv[i], "--define") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_define(argv[i + 1]);
            char* stringParse = strtok(argv[i + 1], "=");

            *variables
                    = realloc(*variables, sizeof(Var) * (*variablesCount + 1));

            (*variables)[*variablesCount].name
                    = malloc(strlen(stringParse) + 1);
            strcpy((*variables)[*variablesCount].name, stringParse);

            (*variables)[*variablesCount].value = atof(strtok(NULL, "="));

            (*variablesCount)++;

        } else if (strcmp(argv[i], "--forloop") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_loop(argv[i + 1]);
            char* stringParse = strtok(argv[i + 1], ",");

            *loops = realloc(*loops, sizeof(Loop) * (*loopsCount + 1));

            (*loops)[*loopsCount].name = malloc(strlen(stringParse) + 1);
            strcpy((*loops)[*loopsCount].name, stringParse);

            double valueStart = atof(strtok(NULL, ","));

            (*loops)[*loopsCount].value = valueStart;
            (*loops)[*loopsCount].start = valueStart;
            (*loops)[*loopsCount].increment = atof(strtok(NULL, ","));
            (*loops)[*loopsCount].end = atof(strtok(NULL, ","));

            (*loopsCount)++;

        } else if (strcmp(argv[i], "--significantfigs") == 0) {
            variable_check_null(argv[i + 1]);
            variable_check_sigfig(argv[i + 1]);

            (*significantFigs) = atoi(argv[i + 1]);
        } else if ((i == argc - 1) && (argv[i][0] != '-')
                && (argv[i - 1][0] != '-') && (strcmp(argv[i], "") != 0)) {
            (*file) = fopen(argv[i], "r");
            if ((*file) == NULL) {
                free_variables(*variables, *variablesCount);
                free_loops(*loops, *loopsCount);
                fprintf(stderr,
                        "uqexpr: unable to open file \"%s\" for reading\n",
                        argv[i]);
                exit(4);
            }
        } else if (argv[i][0] == '-' && argv[i][1] == '-') {
            free_variables(*variables, *variablesCount);
            free_loops(*loops, *loopsCount);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(11);
        } else if (strcmp(argv[i], "") == 0
                || ((i != argc - 1) && (argv[i][0] == '/'))) {
            free_variables(*variables, *variablesCount);
            free_loops(*loops, *loopsCount);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(11);
        }
    }
    // ee
    for (int i = 0; i < *loopsCount; i++) {
        for (int l = 0; l < *loopsCount; l++) {
            if ((i != l) && (strcmp((*loops)[i].name, (*loops)[l].name) == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(5);
            }
        }
    }

    for (int i = 0; i < *variablesCount; i++) {
        for (int l = 0; l < *variablesCount; l++) {
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

te_variable* struct_to_te_var(
        Var** variables, Loop** loops, int* variablesCount, int* loopsCount)
{
    int varCountChecked;
    if (variablesCount != NULL) {
        varCountChecked = (*variablesCount);
    } else {
        varCountChecked = 0;
    }

    int loopCountChecked;
    if (loopsCount != NULL) {
        loopCountChecked = (*loopsCount);
    } else {
        loopCountChecked = 0;
    }

    int total = varCountChecked + loopCountChecked;
    te_variable* result = (te_variable*)malloc(sizeof(te_variable) * (total));
    for (int i = 0; i < varCountChecked; i++) {
        result[i].name = (*variables)[i].name;
        result[i].address = &((*variables)[i].value);
    }

    for (int i = 0; i < loopCountChecked; i++) {
        result[varCountChecked + i].name = (*loops)[i].name;
        result[varCountChecked + i].address = &((*loops)[i].value);
    }

    return result;
}

void main_handler(FILE* file, int* significantFigs, Var** variables,
        int* variablesCount, Loop** loops, int* loopsCount)
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
        } else if (strcmp(line, "@print") == 0) {
            if (*variables == NULL) {
                fprintf(stdout, "There are no variables.\n");
            } else {
                fprintf(stdout, "Variables:\n");
                for (int i = 0; i < (*variablesCount); i++) {
                    fprintf(stdout, "%s = %.*g\n", (*variables)[i].name,
                            *significantFigs, (*variables)[i].value);
                }
            }

            if (*loops == NULL) {
                fprintf(stdout, "There are no loop variables.\n");
            } else {
                fprintf(stdout, "Loop variables:\n");
                for (int i = 0; i < (*loopsCount); i++) {
                    fprintf(stdout, "%s = %.*g (%.*g, %.*g, %.*g)\n",
                            (*loops)[i].name, *significantFigs,
                            (*loops)[i].value, *significantFigs,
                            (*loops)[i].start, *significantFigs,
                            (*loops)[i].increment, *significantFigs,
                            (*loops)[i].end);
                }
            }
        } else if (strcmp(strtok(strdup(line), " "), "@range") == 0) {
            for (int i = 0; i < 6; i++) {
                if (line[i] == ' '
                        || (line[i + 1] == ' ' && line[i + 2] == ' ')) {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                    continue;
                }
            }
            char* loop_var = strtok(NULL, " ");
            int check = loop_check_op(loop_var);

            if (check == 1) {
                continue;
            }
            char* stringParse = strtok(loop_var, ",");
            double valueStart = atof(strtok(NULL, ","));
            double value_increment = atof(strtok(NULL, ","));
            double value_end = atof(strtok(NULL, ","));

            int var_exists = 0;
            for (int i = 0; i < (*variablesCount); i++) {
                if (strcmp((*variables)[i].name, stringParse) == 0) {
                    free((*variables)[i].name);

                    for (int l = i; l < (*variablesCount) - 1; l++) {
                        (*variables)[l] = (*variables)[l + 1];
                    }

                    (*variablesCount)--;

                    *variables = realloc(
                            *variables, sizeof(Var) * (*variablesCount));
                    break;
                }
            }

            for (int i = 0; i < (*loopsCount); i++) {
                if (strcmp((*loops)[i].name, stringParse) == 0) {
                    (*loops)[i].value = valueStart;
                    (*loops)[i].start = valueStart;
                    (*loops)[i].increment = value_increment;
                    (*loops)[i].end = value_end;
                    var_exists = 1;
                    break;
                }
            }

            if (!var_exists) {
                *loops = realloc(*loops, sizeof(Loop) * (*loopsCount + 1));

                (*loops)[*loopsCount].name = malloc(strlen(stringParse) + 1);
                strcpy((*loops)[*loopsCount].name, stringParse);
                (*loops)[*loopsCount].value = valueStart;
                (*loops)[*loopsCount].start = valueStart;
                (*loops)[*loopsCount].increment = value_increment;
                (*loops)[*loopsCount].end = value_end;
                (*loopsCount)++;
            }

        } else {
            char** separated_line = separate_line(line, '=');

            if (separated_line[1] == NULL) {
                int error_compile = 0;

                te_variable* vars = struct_to_te_var(
                        variables, loops, variablesCount, loopsCount);

                te_expr* expr = te_compile(line, vars,
                        (*variablesCount) + (*loopsCount), &error_compile);
                double result = te_eval(expr);
                if (error_compile == 0) {
                    fprintf(stdout, "Result = %.*g\n", *significantFigs,
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

                char* trimmed_lhs = malloc(strlen(lhs) + 1);
                for (size_t i = 0; i < strlen(lhs); i++) {
                    if (!isspace(lhs[i])) {
                        size_t length = strlen(trimmed_lhs);
                        trimmed_lhs[length] = lhs[i];
                        trimmed_lhs[length + 1] = '\0';
                    }
                }
                int check = variable_check_name_op(trimmed_lhs);

                if (check == 1) {
                    continue;
                }

                te_variable* vars = struct_to_te_var(
                        variables, loops, variablesCount, loopsCount);
                te_expr* expr = te_compile(rhs, vars,
                        (*variablesCount) + (*loopsCount), &error_compile);

                if (error_compile != 0) {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                } else {
                    double result = 0;
                    int var_exists = 0;
                    for (int i = 0; i < (*variablesCount); i++) {
                        if (strcmp((*variables)[i].name, trimmed_lhs) == 0) {
                            result = te_eval(expr);
                            (*variables)[i].value = result;
                            var_exists = 1;
                            break;
                        }
                    }

                    for (int i = 0; i < (*loopsCount); i++) {
                        if (strcmp((*loops)[i].name, trimmed_lhs) == 0) {
                            result = te_eval(expr);
                            (*loops)[i].value = result;
                            var_exists = 1;
                            break;
                        }
                    }

                    if (!var_exists) {
                        *variables = realloc(*variables,
                                sizeof(Var) * (*variablesCount + 1));

                        (*variables)[*variablesCount].name
                                = malloc(strlen(trimmed_lhs) + 1);
                        strcpy((*variables)[*variablesCount].name,
                                trimmed_lhs);

                        result = te_eval(expr);

                        (*variables)[*variablesCount].value = result;

                        (*variablesCount)++;
                    }

                    fprintf(stdout, "%s = %.*g\n", trimmed_lhs,
                            *significantFigs, result);
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

    int variablesCount = 0;
    int loopsCount = 0;
    int significantFigs = 4;

    command_arg_check(argc, argv, &variables, &loops, &variablesCount,
            &loopsCount, &significantFigs, &file);

    fprintf(stdout,
            "Welcome to uqexpr.\nThis program was written by s4808239.\n");

    if (variables == NULL) {
        fprintf(stdout, "There are no variables.\n");
    } else {
        fprintf(stdout, "Variables:\n");
        for (int i = 0; i < variablesCount; i++) {
            fprintf(stdout, "%s = %.*g\n", variables[i].name, significantFigs,
                    variables[i].value);
        }
    }

    if (loops == NULL) {
        fprintf(stdout, "There are no loop variables.\n");
    } else {
        fprintf(stdout, "Loop variables:\n");
        for (int i = 0; i < loopsCount; i++) {
            fprintf(stdout, "%s = %.*g (%.*g, %.*g, %.*g)\n", loops[i].name,
                    significantFigs, loops[i].value, significantFigs,
                    loops[i].start, significantFigs, loops[i].increment,
                    significantFigs, loops[i].end);
        }
    }

    if (file == NULL) {
        fprintf(stdout,
                "Please enter your expressions and assignment operations to be "
                "evaluated.\n");
        main_handler(stdin, &significantFigs, &variables, &variablesCount,
                &loops, &loopsCount);
    } else {
        main_handler(file, &significantFigs, &variables, &variablesCount,
                &loops, &loopsCount);
    }

    return 0;
}
