#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* name;
    double value;
} Var;

Var variables[10] //Change to dynamically allocate memory;

void command_arg_check(int argc, char** argv) {
     
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--define") == 0) {
            printf("Includes define\n");

            char* string_parse = strtok(argv[i+1], "=");
            
            variables[0].name = malloc(strlen(string_parse)+1);
            strcpy(variables[0].name, string_parse);

            variables[0].value = atof(strtok(NULL, "="));
            
            printf("%s = %f", variables[0].name, variables[0].value);
        } else if (strcmp(argv[i], "--forloop") == 0) {
            printf("Includes for loop\n");
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

