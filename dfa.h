
#ifndef SP5_DFA_H
#define SP5_DFA_H


#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STATES 64
#define MAX_ALPHABET 26

typedef struct {
    int alphabet_size;
    int number_of_states;
    int start_state;
    int size_final_states;
    int **transitions;
    int **partition_transitions;
    long int reachable;
    long int all_states;
    long int final_states;
    long int non_final_states;
    long int *P;
    int stateMapping[MAX_STATES];
} DFA;


void dfs(DFA *dfa, int v);
void determine_reachable_states(DFA *dfa);
void initialise_transition_map(DFA *dfa);
void partition_dfa(DFA *dfa);
void print_dfa_details(DFA *dfa, FILE* file);
void initialise_dfa(DFA *dfa, FILE *file);
void print_dfa_original(DFA *dfa, FILE* file);

#endif 