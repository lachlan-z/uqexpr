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

// initialise pointer to variables for dynamic memory allocation later 
Var* variables = NULL;
Loop* loops = NULL;

void command_arg_check(int argc, char** argv) {
     
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--define") == 0) {
            // add error checking
            printf("Includes define\n");

            char* string_parse = strtok(argv[i+1], "=");
            
            variables = realloc(variables, sizeof(Var) + 1);

            variables[0].name = malloc(strlen(string_parse)+1);
            strcpy(variables[0].name, string_parse);

            variables[0].value = atof(strtok(NULL, "="));
            
            printf("%s = %f", variables[0].name, variables[0].value);
        } else if (strcmp(argv[i], "--forloop") == 0) {
            printf("Includes for loop\n");

            char* string_parse = strtok(argv[i+1], ",");

            loops = realloc(loops, sizeof(Loop) + 1);
 
            loops[0].name = malloc(strlen(string_parse)+1);
            strcpy(loops[0].name, string_parse);
 
            loops[0].start = atof(strtok(NULL, ","));
            loops[0].increment = atof(strtok(NULL, ","));
            loops[0].end = atof(strtok(NULL, ","));

            printf("name:%s start:%f increment:%f end:%f\n", loops[0].name, loops[0].start, loops[0].increment, loops[0].end);
        } else if (strcmp(argv[i], "--significantfigs") == 0) {
            printf("Includes significant figs\n");
        }
    }
}


int main(int argc, char** argv) {
    
    printf("Welcome to uqexpr.\nThis program was writted by s4808239.\n");
    command_arg_check(argc, argv);
    return 0;

}

