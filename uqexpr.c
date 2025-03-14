#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    
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

void command_arg_check(int argc, char** argv, Var** variables, Loop** loops, int* variables_count, int* loops_count) {     
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--define") == 0) {
            // add error checking
            printf("Includes define\n");

            char* string_parse = strtok(argv[i+1], "=");
            
            *variables = realloc(*variables, sizeof(Var) * (*variables_count + 1));

            (*variables)[*variables_count].name = malloc(strlen(string_parse) + 1);
            strcpy((*variables)[*variables_count].name, string_parse);

            (*variables)[*variables_count].value = atof(strtok(NULL, "="));  
            
            (*variables_count)++;
            
            for (int l = 0; l < *variables_count; l++) {
                printf("variables[%d]: %s = %f", l, (*variables)[l].name, (*variables)[l].value);
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
                printf("loops[%d]: %s = %f", l, (*loops)[l].name, (*loops)[l].start), (*loops)[l].increment, (*loops)[l].end;
            }
        } else if (strcmp(argv[i], "--significantfigs") == 0) {
            printf("Includes significant figs\n");
        }
    }
}

int main(int argc, char** argv) {
    Var* variables = NULL; 
    Loop* loops = NULL;                                                              
                                                                                   
    int variables_count = 0;                                                         
    int loops_count = 0;  

    printf("Welcome to uqexpr.\nThis program was writted by s4808239.\n");
    command_arg_check(argc, argv, &variables, &loops, &variables_count, &loops_count);
    return 0;

}

