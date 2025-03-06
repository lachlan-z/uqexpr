#include <stdio.h>
#include <string.h>

void command_arg_check(int argc, char** argv) {
    
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--define") == 0) {
            printf("Includes define\n");
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

