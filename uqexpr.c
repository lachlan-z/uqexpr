#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
    
typedef struct {                                                                 
    char* name;                                                                  
    double value;                                                                
} Var;

typedef struct {                                                                 
    char* name;                                                                  
    double start;                                                                
    double increment;                                                            
    double end;                                                                  
} Loop; 

void variable_check_null(char* variable) {
    if (variable == NULL) {
        fprintf(stderr, "Usage: ./uqexpr [--forloop string] [--define string] [--significantfigs 2..8] [inputfile]\n");
        exit(11);
    }
}

void variable_check_name(char* variable_name) {
    if (!((size_t)1 <= strlen(variable_name) && strlen(variable_name) <= (size_t)25)) {
        fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    for (size_t l = 0; l < strlen(variable_name); l++) {
        if (isalpha(variable_name[l]) == 0) {
            fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
            exit(12);
        }
    }
}

void variable_check_define(char* variable) {
    char* string_parse = strtok(strdup(variable), "=");
    variable_check_name(string_parse);
}

void variable_check_loop(char* variable) {
    char* string_parse = strtok(strdup(variable), ",");
    variable_check_name(string_parse);
    
    double start = atof(strtok(NULL, ","));
    double increment = atof(strtok(NULL, ","));
    double end = atof(strtok(NULL, ","));

    if ((increment == 0) || ((start < end) && (increment < 0)) || ((start > end) && (increment > 0))) {
        fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }    
}

void variable_check_sigfig(char* variable) {
    if ((atof(variable) < 2) || (atof(variable) > 8)) { 
        fprintf(stderr, "Usage: ./uqexpr [--forloop string] [--define string] [--significantfigs 2..8] [inputfile]\n");
        exit(11);
    }
}

void command_arg_check(int argc, char** argv, Var** variables, Loop** loops, int* variables_count, int* loops_count, int* significant_figs) {     
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
        if (strcmp(argv[i], "--define") == 0) {
            // add error checking
            printf("Includes define\n");
            variable_check_null(argv[i+1]);
            variable_check_define(argv[i+1]);
            char* string_parse = strtok(argv[i+1], "=");
            
            *variables = realloc(*variables, sizeof(Var) * (*variables_count + 1));

            (*variables)[*variables_count].name = malloc(strlen(string_parse) + 1);
            strcpy((*variables)[*variables_count].name, string_parse);

            (*variables)[*variables_count].value = atof(strtok(NULL, "="));  
            
            (*variables_count)++;
            
            for (int l = 0; l < *variables_count; l++) {
                printf("variables[%d]: %s = %f\n", l, (*variables)[l].name, (*variables)[l].value);
            }
        } else if (strcmp(argv[i], "--forloop") == 0) {
            // add error checking
            printf("Includes for loop\n");
            variable_check_null(argv[i+1]);
            variable_check_loop(argv[i+1]); 
            char* string_parse = strtok(argv[i+1], ",");

            *loops = realloc(*loops, sizeof(Loop) * (*loops_count + 1));
 
            (*loops)[*loops_count].name = malloc(strlen(string_parse) + 1);
            strcpy((*loops)[*loops_count].name, string_parse);
 
            (*loops)[*loops_count].start = atof(strtok(NULL, ","));
            (*loops)[*loops_count].increment = atof(strtok(NULL, ","));
            (*loops)[*loops_count].end = atof(strtok(NULL, ","));

            (*loops_count)++;

            for (int l = 0; l < *loops_count; l++) {
                printf("loops[%d]: name:%s start:%f inc:%f end:%f\n", l, (*loops)[l].name, (*loops)[l].start, (*loops)[l].increment, (*loops)[l].end);
            }
        } else if (strcmp(argv[i], "--significantfigs") == 0) {
            variable_check_null(argv[i+1]);
            variable_check_sigfig(argv[i+1]);
            printf("Includes significant figs\n");
            (*significant_figs) = atoi(argv[i+1]);
        } else if ((i > 0) && (argv[i][0] != '-') && (argv[i-1][0] != '-') && (i != 0)) {
            printf("filename check included");
            FILE* file = fopen(argv[i], "r");
            if (file == NULL) {
                fprintf(stderr, "uqexpr: unable to open file \"%s\" for reading\n", argv[i]);
                exit(4);
            }
        } 
    }
    
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
            if ((i != l) && (strcmp((*variables)[i].name, (*variables)[l].name) == 0)) {
                fprintf(stderr, "uqexpr: duplicate variables were detected\n");
                exit(5);
            }
        }
    }
}

int main(int argc, char** argv) {
    Var* variables = NULL; 
    Loop* loops = NULL;                                                              
                                                                                   
    int variables_count = 0;                                                         
    int loops_count = 0;
    // check functionality of sigfigs in demo-uqexpr
    int significant_figs = 4;

    command_arg_check(argc, argv, &variables, &loops, &variables_count, &loops_count, &significant_figs);
   
    fprintf(stdout, "Welcome to uqexpr.\nThis program was writted by s4808239.\n");
    
    if (variables == NULL) {
        fprintf(stdout, "There are no variables.\n");
    } else {
        fprintf(stdout, "Variables:\n");
        for (int i = 0; i < variables_count; i++) {
            fprintf(stdout, "%s = %.*g\n", variables[i].name, significant_figs, variables[i].value);
        }
    }
    
    return 0;
}

