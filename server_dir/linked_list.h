#pragma once
#include <stdio.h>
#include <stdlib.h>	 
#include <string.h>

#define BUFFSIZE 8192

//Every node of this list has the voter name and the party who voted 
typedef struct Node {
    char voter_name[BUFFSIZE];
    char party[BUFFSIZE];
    struct Node *next;
} Node;

//This list has all the different parties we found in the poll
typedef struct party_list {
    char party[BUFFSIZE];
    struct party_list *next;
} party_list;

int initialize_linked_list(Node **list);
int destroy_linked_list(Node **list);
int push(Node **list, char *voter_name, char *party);
int find_voter(Node *list, char *voter_name);
int print_list(Node *list);

int initialize_party_list(party_list **list);
int destroy_party_list(party_list **list);
int exist_party(party_list *list, char *party_name);
int new_party_add(party_list **list, char *party_name);
int number_of_parties(party_list *list);
int print_party_list(party_list *list);