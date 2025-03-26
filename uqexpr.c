#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <tinyexpr.h>

#define INVALID_VAR 12
#define USAGE_ERR 11
#define INVALID_FILE 4
#define DUPLICATE_VAR 5
#define MAX_VAR_LEN 25
#define MIN_VAR_LEN 1
#define MAX_SIGFIG 8
#define MIN_SIGFIG 2
#define DEFAULT_SIGFIG 4
#define RANGE_LEN 6

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

void variable_check_null(const char* variable)
{
    if (variable == NULL) {
        fprintf(stderr,
                "Usage: ./uqexpr [--forloop string] [--define string] "
                "[--significantfigs 2..8] [inputfilename]\n");
        exit(USAGE_ERR);
    }
}

void variable_check_name(char* variableName)
{
    if (!((size_t)MIN_VAR_LEN <= strlen(variableName)
                && strlen(variableName) <= (size_t)MAX_VAR_LEN)) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(INVALID_VAR);
    }

    for (size_t l = 0; l < strlen(variableName); l++) {
        if (isalpha(variableName[l]) == 0) {
            fprintf(stderr,
                    "uqexpr: invalid variable(s) specified on the command "
                    "line\n");
            exit(INVALID_VAR);
        }
    }
}

int variable_check_name_op(char* variableName)
{
    if (!((size_t)MIN_VAR_LEN <= strlen(variableName)
                && strlen(variableName) <= (size_t)MAX_VAR_LEN)) {
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
            exit(INVALID_VAR);
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
            exit(INVALID_VAR);
        }
    }

    if (stringParse == NULL || value == NULL) {
        fprintf(stderr,
                "uqexpr: invalid variable(s) specified on the command line\n");
        exit(INVALID_VAR);
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
        exit(INVALID_VAR);
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
        exit(INVALID_VAR);
    }
}

void variable_check_sigfig(char* variable)
{
    if ((variable[0] == '0') || (atof(variable) < MIN_SIGFIG)
            || (atof(variable) > MAX_SIGFIG)) {
        fprintf(stderr,
                "Usage: ./uqexpr [--forloop string] [--define string] "
                "[--significantfigs 2..8] [inputfilename]\n");
        exit(USAGE_ERR);
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
        int* variablesCount, int* loopsCount, int* significantFigs, FILE** file)
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
                exit(INVALID_FILE);
            }
        } else if (argv[i][0] == '-' && argv[i][1] == '-') {
            free_variables(*variables, *variablesCount);
            free_loops(*loops, *loopsCount);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(USAGE_ERR);
        } else if (strcmp(argv[i], "") == 0
                || ((i != argc - 1) && (argv[i][0] == '/'))) {
            free_variables(*variables, *variablesCount);
            free_loops(*loops, *loopsCount);
            fprintf(stderr,
                    "Usage: ./uqexpr [--forloop string] [--define string] "
                    "[--significantfigs 2..8] [inputfilename]\n");
            exit(USAGE_ERR);
        }
    }
    // ee
    for (int i = 0; i < *loopsCount; i++) {
        for (int l = 0; l < *loopsCount; l++) {
            if ((i != l) && (strcmp((*loops)[i].name, (*loops)[l].name) == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(DUPLICATE_VAR);
            }
        }
    }

    for (int i = 0; i < *variablesCount; i++) {
        for (int l = 0; l < *variablesCount; l++) {
            if ((i != l)
                    && (strcmp((*variables)[i].name, (*variables)[l].name)
                            == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(DUPLICATE_VAR);
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
    char** tokens = (char**)malloc(sizeof(char*));
    tokens[0] = NULL;

    char delimiters[] = {delim, '\0'};

    int numTokens = 0;
    char* token = strtok(string, delimiters);

    while (token) {
        tokens[numTokens] = strdup(token);
        numTokens++;
        tokens = (char**)realloc(
                (void*)tokens, sizeof(char*) * (numTokens + 1));
        tokens[numTokens] = NULL;

        token = strtok(NULL, delimiters);
    }

    return tokens;
}

te_variable* struct_to_te_var(Var** variables, Loop** loops,
        const int* variablesCount, const int* loopsCount)
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

void main_handler(FILE* file, const int* significantFigs, Var** variables,
        int* variablesCount, Loop** loops, int* loopsCount)
{
    while (1) {
        char* line = read_line(file);
        if (feof(file)) {
            fprintf(stdout, "Thank you for using uqexpr.\n");
            exit(0);
        }

        if (line[0] == '#') {
            continue;
        }

        if (strcmp(line, "@print") == 0) {
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
            for (int i = 0; i < RANGE_LEN; i++) {
                if (line[i] == ' '
                        || (line[i + 1] == ' ' && line[i + 2] == ' ')) {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                    continue;
                }
            }
            char* loopVar = strtok(NULL, " ");
            int check = loop_check_op(loopVar);

            if (check == 1) {
                continue;
            }
            char* stringParse = strtok(loopVar, ",");
            double valueStart = atof(strtok(NULL, ","));
            double valueIncrement = atof(strtok(NULL, ","));
            double valueEnd = atof(strtok(NULL, ","));

            int varExists = 0;
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
                    (*loops)[i].increment = valueIncrement;
                    (*loops)[i].end = valueEnd;
                    varExists = 1;
                    break;
                }
            }

            if (!varExists) {
                *loops = realloc(*loops, sizeof(Loop) * (*loopsCount + 1));

                (*loops)[*loopsCount].name = malloc(strlen(stringParse) + 1);
                strcpy((*loops)[*loopsCount].name, stringParse);
                (*loops)[*loopsCount].value = valueStart;
                (*loops)[*loopsCount].start = valueStart;
                (*loops)[*loopsCount].increment = valueIncrement;
                (*loops)[*loopsCount].end = valueEnd;
                (*loopsCount)++;
            }

        } else {
            char** separatedLine = separate_line(line, '=');

            if (separatedLine[1] == NULL) {
                int errorCompile = 0;

                te_variable* vars = struct_to_te_var(
                        variables, loops, variablesCount, loopsCount);

                te_expr* expr = te_compile(line, vars,
                        (*variablesCount) + (*loopsCount), &errorCompile);
                double result = te_eval(expr);
                if (errorCompile == 0) {
                    fprintf(stdout, "Result = %.*g\n", *significantFigs,
                            result);
                } else {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                }
            } else {
                char* lhs = separatedLine[0];
                char* rhs = separatedLine[1];
                int errorCompile = 0;

                char* trimmedLhs = malloc(strlen(lhs) + 1);
                for (size_t i = 0; i < strlen(lhs); i++) {
                    if (!isspace(lhs[i])) {
                        size_t length = strlen(trimmedLhs);
                        trimmedLhs[length] = lhs[i];
                        trimmedLhs[length + 1] = '\0';
                    }
                }
                int check = variable_check_name_op(trimmedLhs);

                if (check == 1) {
                    continue;
                }

                te_variable* vars = struct_to_te_var(
                        variables, loops, variablesCount, loopsCount);
                te_expr* expr = te_compile(rhs, vars,
                        (*variablesCount) + (*loopsCount), &errorCompile);

                if (errorCompile != 0) {
                    fprintf(stderr,
                            "Invalid command, expression or assignment "
                            "operation\n");
                } else {
                    double result = 0;
                    int varExists = 0;
                    for (int i = 0; i < (*variablesCount); i++) {
                        if (strcmp((*variables)[i].name, trimmedLhs) == 0) {
                            result = te_eval(expr);
                            (*variables)[i].value = result;
                            varExists = 1;
                            break;
                        }
                    }

                    for (int i = 0; i < (*loopsCount); i++) {
                        if (strcmp((*loops)[i].name, trimmedLhs) == 0) {
                            result = te_eval(expr);
                            (*loops)[i].value = result;
                            varExists = 1;
                            break;
                        }
                    }

                    if (!varExists) {
                        *variables = realloc(*variables,
                                sizeof(Var) * (*variablesCount + 1));

                        (*variables)[*variablesCount].name
                                = malloc(strlen(trimmedLhs) + 1);
                        strcpy((*variables)[*variablesCount].name, trimmedLhs);

                        result = te_eval(expr);

                        (*variables)[*variablesCount].value = result;

                        (*variablesCount)++;
                    }

                    fprintf(stdout, "%s = %.*g\n", trimmedLhs, *significantFigs,
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

    int variablesCount = 0;
    int loopsCount = 0;
    int significantFigs = DEFAULT_SIGFIG;

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
