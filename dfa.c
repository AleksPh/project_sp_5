#include "dfa.h"
#include <stdio.h>
#include <stdlib.h>

void dfs(DFA *dfa, int v) {
    dfa->reachable |= (1 << v);

    for (int i = 0; i < MAX_ALPHABET; i++)
        if ((dfa->transitions[v][i] != -1) && ((dfa->reachable & (1 << dfa->transitions[v][i])) == 0))
            dfs(dfa, dfa->transitions[v][i]);
}

void determine_reachable_states(DFA *dfa) {
    dfa->reachable = 0;
    dfs(dfa,dfa->start_state);
    dfa->all_states &= dfa->reachable;
    dfa->final_states &= dfa->reachable;
    dfa->non_final_states = dfa->all_states & ~dfa->final_states;

}

void initialise_transition_map(DFA *dfa) {
    dfa->transitions = (int **)malloc(MAX_STATES * sizeof(int *));
    dfa->partition_transitions = (int **)malloc(MAX_STATES * sizeof(int *));

    for (int i = 0; i < MAX_STATES; i++) {
        dfa->transitions[i] = (int *)malloc(MAX_ALPHABET * sizeof(int));
        dfa->partition_transitions[i] = (int *)malloc(MAX_ALPHABET * sizeof(int));

        for (int j = 0; j < MAX_ALPHABET; j++) {
            dfa->transitions[i][j] = -1;
            dfa->partition_transitions[i][j] = -1;
        }
    }
}

//void partition_dfa(DFA *dfa) {
//    dfa->P = (long int *)malloc(MAX_STATES * sizeof(long int));
//    for (int i = 0; i < MAX_STATES; i++)
//        dfa->P[i] = 0;
//
//    dfa->P[0] = dfa->final_states;
//    dfa->P[1] = dfa->non_final_states;
//
//
//
//    int nextPartitionIndex = 2;
//
//    for (int i = 0; i < MAX_STATES; i++) {
//        printf("index = %d, dfa->p[i] = %d \n", i, dfa->P[i]);
//        if (dfa->P[i] == 0)
//            break;
//
//        long int newPartition = 0;
//        for (int j = MAX_STATES - 1; j >= 0; j--) {
//            long int staticState = (long int) 1 << j;
//
//            if ((dfa->P[i] & (staticState)) != 0) {
//                dfa->partition_transitions[i] = dfa->transitions[j];
//
//                for (int k = j - 1; k >= 0; k--) {
//                    long int otherState = (long int) 1 << k;
//                    if ((dfa->P[i] & (otherState)) != 0) {
//                        for (int l = 0; l < MAX_ALPHABET; l++) {
//                            int staticNext = -1;
//                            int otherNext = -1;
//
//                            for (int m = 0; m < nextPartitionIndex; m++) {
//                                if ((dfa->P[m] & (1 << dfa->transitions[j][l])) != 0)
//                                    staticNext = m;
//                                if ((dfa->P[m] & (1 << dfa->transitions[k][l])) != 0)
//                                    otherNext = m;
//                            }
//
//                            if (dfa->transitions[j][l] != dfa->transitions[k][l] && (staticNext != otherNext)) {
//                                dfa->P[i] &= ~(1 << k);
//                                newPartition |= (1 << k);
//                                break;
//                            }
//                        }
//                    }
//                }
//                break;
//            }
//        }
//
//        if (newPartition != 0) {
//            dfa->P[nextPartitionIndex] = newPartition;
//            printf("newpartition %d \n", newPartition);
//            nextPartitionIndex++;
//        }
//    }
//}
void partition_dfa(DFA *dfa) {
    dfa->P = (long int *)malloc(MAX_STATES * sizeof(long int));
    for (int i = 0; i < MAX_STATES; i++)
        dfa->P[i] = 0;

    dfa->P[0] = dfa->final_states;
    dfa->P[1] = dfa->non_final_states;

    int nextPartitionIndex = 2;

    // Create a mapping from original state numbers to their corresponding numbers in the minimized DFA
    int stateMapping[MAX_STATES];
    for (int i = 0; i < MAX_STATES; i++) {
        stateMapping[i] = i;
        dfa->stateMapping[i] = stateMapping[i];
    }

    for (int i = 0; i < MAX_STATES; i++) {
        //printf("Original state numbers in partition %d: ", i);
        for (int k = 0; k < dfa->number_of_states; k++) {
            if ((dfa->P[i] & (1 << k)) != 0) {
                //printf("%d ", stateMapping[k]);
                stateMapping[k] = i;
                dfa->stateMapping[k] = stateMapping[k];
            }
        }
        //printf("\n");

        long int newPartition = 0;
        for (int j = MAX_STATES - 1; j >= 0; j--) {
            long int staticState = (long int) 1 << j;

            if ((dfa->P[i] & (staticState)) != 0) {
                dfa->partition_transitions[i] = dfa->transitions[j];

                for (int k = j - 1; k >= 0; k--) {
                    long int otherState = (long int) 1 << k;
                    if ((dfa->P[i] & (otherState)) != 0) {
                        for (int l = 0; l < MAX_ALPHABET; l++) {
                            int staticNext = -1;
                            int otherNext = -1;

                            for (int m = 0; m < nextPartitionIndex; m++) {
                                if ((dfa->P[m] & (1 << dfa->transitions[j][l])) != 0)
                                    staticNext = m;
                                if ((dfa->P[m] & (1 << dfa->transitions[k][l])) != 0)
                                    otherNext = m;
                            }

                            if (dfa->transitions[j][l] != dfa->transitions[k][l] && (staticNext != otherNext)) {
                                dfa->P[i] &= ~(1 << k);
                                newPartition |= (1 << k);
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }

        if (newPartition != 0) {
           // printf("Minimized state numbers in new partition %d: ", nextPartitionIndex);
            for (int k = 0; k < MAX_STATES; k++) {
                if ((newPartition & (1 << k)) != 0) {
                    //printf("%d ", nextPartitionIndex);
                    stateMapping[k] = nextPartitionIndex;
                    dfa->stateMapping[k] = stateMapping[k];// Update the state mapping for the new partition
                }
            }
            //printf("\n");

            dfa->P[nextPartitionIndex] = newPartition;
            nextPartitionIndex++;
        }
    }
}
void print_dfa_original(DFA *dfa, FILE* file) {
    printf("--------------\n");
    fprintf(file, "--------------\n");
    printf("Mapping States: ");
    fprintf(file,"Mapping States: ");
    for (int i = 0; i < dfa->number_of_states; i++) {
        printf("%d ", dfa->stateMapping[i]);
        fprintf(file, "%d ", dfa->stateMapping[i]);
    }
    printf("\n");
    fprintf(file, "\n");
    printf("Original States: ");
    fprintf(file, "Original States: ");
    for (int i = 0; i < dfa->number_of_states; i++) {
        printf("%d ", i);
        fprintf(file, "%d ", i);
    }
    printf("\n");
    fprintf(file, "\n");
    printf("--------------\n");
    fprintf(file, "--------------\n");
    printf("Original Numerated Minimised DFA: \n");
    fprintf(file, "Original Numerated Minimised DFA: \n");
    int startPartition = 0;
    for (int i = 0; i < dfa->number_of_states; i++) {
        if ((dfa->P[i] & (1 << dfa->start_state)) != 0) {
            startPartition = i;
            break;
        }
    }

    printf("Alphabet Size: %d \n", dfa->alphabet_size);
    fprintf(file,"Alphabet Size: %d \n", dfa->alphabet_size);
    int x = 0;
    for (int i = 0; i < dfa->number_of_states; i++) {
        for (int j = 0; j < MAX_ALPHABET; j++) {
            if (dfa->partition_transitions[i][j] != -1) {
                for (int k = 0; k < dfa->number_of_states; k++) {
                    if ((dfa->P[k] & (1 << dfa->partition_transitions[i][j])) != 0)
                        x += 1;
                }
            }
        }
    }
    printf("Number of States: %d \n", x / 2);
    fprintf(file,"Number of States: %d \n", x / 2);
    int new_states[x/2];
    for (int i = 0; i < x/2; i++){
        new_states[i] = 1000;
    }
    for (int i = 0; i < dfa->number_of_states; i++){
        if (new_states[dfa->stateMapping[i]] < i) {
            new_states[dfa->stateMapping[i]] = new_states[dfa->stateMapping[i]];
        }
        else{
            new_states[dfa->stateMapping[i]] = i;
        }
    }
    printf("Results States: ");
    for (int i = 0; i < x/2; i++) {
        printf("%d ", new_states[i]);
    }
    printf("\n");
    char symbols[x/2];
    int current_symbol = x/2 - 1;

    for (int i = 0; i < x/2; i++) {
        symbols[i] = current_symbol;
        current_symbol--;
    }

    printf("Start State: %d \n", new_states[startPartition]);
    fprintf(file,"Start State: %d \n", new_states[startPartition]);
    int x1 = 0;

    for (int i = 0; i < dfa->number_of_states; i++) {
        if ((dfa->P[i] & dfa->final_states) != 0)
            x1 += 1;
        // printf("%d ", i);
    }
    fprintf(file,"Number of Final States: %d \n", x1);
    printf("Number of Final States: %d \n", x1);
    fprintf(file,"Final States:");
    printf("Final States:");

    for (int i = 0; i < dfa->number_of_states; i++) {
        if ((dfa->P[i] & dfa->final_states) != 0) {
            printf("%d ", new_states[i]);
            fprintf(file, "%d ", new_states[i]);
        }
    }

    fprintf(file,"\n");
    printf("\n");

    for (int i = dfa->number_of_states - 1; i >= 0; i--) {
        for (int j = 0; j < dfa->number_of_states ; j++) {
            if (dfa->partition_transitions[i][j] != -1) {
                for (int k = dfa->number_of_states - 1; k >= 0; k--) {
                    if ((dfa->P[k] & (1 << dfa->partition_transitions[i][j])) != 0) {
                        printf("%d %c %d\n", new_states[i], j + 'a', new_states[k]);
                        fprintf(file, "%d %c %d\n", new_states[i], j + 'a', new_states[k]);
                    }
                }
            }
        }
    }
}

void print_dfa_details(DFA *dfa, FILE* file) {
    printf("Minimised DFA: \n");
    int startPartition = 0;
    for (int i = 0; i < MAX_STATES; i++) {
        if ((dfa->P[i] & (1 << dfa->start_state)) != 0) {
            startPartition = i;
            break;
        }
    }
    printf("Alphabet Size: %d \n", dfa->alphabet_size);
    fprintf(file,"Alphabet Size: %d \n", dfa->alphabet_size);
    int x = 0;
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_ALPHABET; j++) {
            if (dfa->partition_transitions[i][j] != -1) {
                for (int k = 0; k < MAX_STATES; k++) {
                    if ((dfa->P[k] & (1 << dfa->partition_transitions[i][j])) != 0)
                        x += 1;
                }
            }
        }
    }
    printf("Number of States: %d \n", x / 2);
    fprintf(file,"Number of States: %d \n", x / 2);
    printf("Start State: %d \n", startPartition);
    fprintf(file,"Start State: %d \n", startPartition);
    int x1 = 0;

    for (int i = 0; i < MAX_STATES; i++) {
        if ((dfa->P[i] & dfa->final_states) != 0)
            x1 += 1;
        // printf("%d ", i);
    }
    fprintf(file,"Number of Final States: %d \n", x1);
    printf("Number of Final States: %d \n", x1);
    fprintf(file,"Final States:");
    printf("Final States:");

    for (int i = 0; i < MAX_STATES; i++) {
        if ((dfa->P[i] & dfa->final_states) != 0) {
            printf("%d ", i);
            fprintf(file, "%d ", i);
        }
    }

    fprintf(file,"\n");
    printf("\n");

    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_ALPHABET; j++) {
            if (dfa->partition_transitions[i][j] != -1) {
                for (int k = 0; k < MAX_STATES; k++) {
                    if ((dfa->P[k] & (1 << dfa->partition_transitions[i][j])) != 0) {
                        printf("%d %c %d\n", i, j + 'a', k);
                        fprintf(file, "%d %c %d\n", i, j + 'a', k);
                    }
                }
            }
        }
    }

}

void initialise_dfa(DFA *dfa, FILE *file) {
    fscanf(file, "%d", &dfa->alphabet_size);
    fscanf(file, "%d", &dfa->number_of_states);
    fscanf(file, "%d", &dfa->start_state);
    int number_of_final_states;
    fscanf(file, "%d", &number_of_final_states);

    dfa->size_final_states = number_of_final_states;
    dfa->final_states = 0;
    for (int i = 0; i < number_of_final_states; ++i) {
        int x;
        fscanf(file, "%d", &x);
        dfa->final_states |= 1 << x;
    }
    for (int i = 0; i < dfa->number_of_states; i++) {
        dfa->all_states |= (1 << i);
    }


    int start_state, end_state;
    char input_char;

    while (fscanf(file, "%d %c %d", &start_state, &input_char, &end_state) == 3) {
        dfa->transitions[start_state][input_char - 'a'] = end_state;

    }

}