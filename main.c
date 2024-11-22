#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dfa.h"

#define MAX_WORD_LENGTH 30
#define MAX_FILENAME_LENGTH 100
const char* test_directory = "Tests/";
const char* result_directory = "Results/output";


int main() {
    char input_file_path[MAX_FILENAME_LENGTH];
    char file_name[MAX_FILENAME_LENGTH];

    FILE* input_file = NULL;
    while (input_file == NULL) {
        printf("Enter a file name: ");
        scanf("%s", file_name);
        char file_path[MAX_FILENAME_LENGTH];
        strcpy(file_path, test_directory);
        strcat(file_path, file_name);
        input_file = fopen(file_path, "r");

        if (input_file == NULL) printf("Couldn't open the input file: %s\n", file_path);
        else strcpy(input_file_path, file_path);
    }

    DFA dfa;
    initialise_transition_map(&dfa);
    initialise_dfa(&dfa, input_file);
    printf("DFA initialized successfully.\n");

    determine_reachable_states(&dfa);
    partition_dfa(&dfa);



    char output_file_path[MAX_FILENAME_LENGTH];
    strcpy(output_file_path, result_directory);
    strcat(output_file_path, input_file_path + strlen(test_directory) + 5);

    FILE *output_file;
    output_file = fopen(output_file_path, "w");
    print_dfa_details(&dfa, output_file);
    print_dfa_original(&dfa, output_file);
    printf("DFA details written to the output file: %s\n", output_file_path);
    return 0;
}