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

void variable_check_define(char* variable) {
    char* string_parse = strtok(strdup(variable), "=");

    if (string_parse == NULL) {
        fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }
    
    if (!((size_t)1 <= strlen(string_parse) && strlen(string_parse) <= (size_t)25)) {
        fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
        exit(12);
    }

    for (size_t l = 0; l < strlen(string_parse); l++) {
        if (isalpha(string_parse[l]) == 0) {
            fprintf(stderr, "uqexpr: invalid variable(s) specified on the command line\n");
            exit(12);
        }
    }
}

void command_arg_check(int argc, char** argv, Var** variables, Loop** loops, int* variables_count, int* loops_count, int* significant_figs) {     
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--define") == 0) {
            // add error checking
            printf("Includes define\n");
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
            printf("Includes significant figs\n");
            (*significant_figs) = atoi(argv[i+1]);
        } else if ((i == argc - 1) && (argv[i][0] != '-') && (strchr(argv[i], '.') != NULL)) {
            printf("filename check included");
        } 
    }
}

int main(int argc, char** argv) {
    Var* variables = NULL; 
    Loop* loops = NULL;                                                              
                                                                                   
    int variables_count = 0;                                                         
    int loops_count = 0;
    int significant_figs = 4;

    printf("Welcome to uqexpr.\nThis program was writted by s4808239.\n");
    command_arg_check(argc, argv, &variables, &loops, &variables_count, &loops_count, &significant_figs);
    return 0;

}

