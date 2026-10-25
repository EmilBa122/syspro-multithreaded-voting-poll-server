#include "linked_list.h"

int initialize_linked_list(Node **list)
{
    *list = NULL;
    return 0; 
}

int destroy_linked_list(Node **list)
{
    Node *current = *list;
    Node *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    *list = NULL;
    return 0;
}

int push(Node **list, char *voter_name, char *party)
{
    Node *new_node = malloc(sizeof(Node));
    strcpy(new_node->voter_name, voter_name);
    strcpy(new_node->party, party);
    new_node->next = *list;
    *list = new_node;
    return 0;
}

int find_voter(Node *list, char *voter_name)
{
    int i = 0;
    while (list)
    {
        if (!strcmp(list->voter_name, voter_name))
        {
            return i;
        }
        list = list->next;
    }
    return -1;
}

int print_list(Node *list)
{
    printf("printing list\n\n");
    while (list)
    {
       printf("voter name %s\n party is %s\n", list->voter_name, list->party);
       list = list->next;
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
int initialize_party_list(party_list **list)
{
    *list = NULL;
    return 0; 
}

int destroy_party_list(party_list **list)
{
    party_list *current = *list;
    party_list *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    *list = NULL;
    return 0;
}

int exist_party(party_list *list, char *party_name)
{
    while (list)
    {
        if (!strcmp(list->party, party_name))
        {
            return 1;
        }
        list = list->next;
    }
    return 0;
}

int new_party_add(party_list **list, char *party_name)
{
    party_list *new_node = malloc(sizeof(party_list));
    strcpy(new_node->party, party_name);
    new_node->next = *list;
    *list = new_node;
    return 0;
}

int number_of_parties(party_list *list)
{
    int counter = 0;
    while(list)
    {
        counter++;
        list = list->next;
    }
    return counter;
}

int print_party_list(party_list *list)
{
    printf("printing party list\n\n");
    while (list)
    {
       printf("party name %s\n", list->party);
       list = list->next;
    }
    return 0;
}