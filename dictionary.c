// Implements a dictionary's functionality
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <stdbool.h>

#include "dictionary.h"

#define TRIEWIDTH 27
#define BASEASCIIINDEX 97

typedef struct _node 
{
    bool is_word;
    struct _node* next[TRIEWIDTH];
}
node;

void inittrie(node* new_trie)
{
    new_trie->is_word = false;
    for ( int i = 0; i < TRIEWIDTH; i++ )
        new_trie->next[i] = NULL;
}

int getLetterIndex(const char letter)
{
    // return last slot if it's "'"
    if ( letter == '\'' )
        return 26;

    int baseAsciiIndex = 97;
    int alphaIndex = ( tolower(letter) - baseAsciiIndex ) % 26;
    return alphaIndex;
}

node* trie;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // copy with 'tolower' in tmp var for case insensitive search
    int size = strlen(word);
    char lword[size + 1];
    for ( int i = 0; i < size + 1; i++ )
        lword[i] = tolower(word[i]);

    node* current_ptr = trie;

    for ( int i = 0; i < size; i++ )
    {
        int index = getLetterIndex(lword[i]);
        if ( current_ptr->next[index] == NULL )
        {
            return false;
        } else
        {
            current_ptr = current_ptr->next[index];
        }
    }

    if ( current_ptr->is_word == true )
    {
        return true;
    }

    return false;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    trie = malloc(sizeof(node));
    if ( trie == NULL )
    {
        unload();
        return false;
    }
    inittrie(trie);

    // open dictionary file
    FILE *dictptr = fopen(dictionary, "r");
    if (dictptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", dictionary);
        return false;
    }

    char word[LENGTH + 1];
    while ( fscanf(dictptr, "%s", word) != EOF )
    {
        node* current_ptr = trie;

        for ( int i = 0; i < strlen(word) ; i++ )
        {
            int index = getLetterIndex(word[i]);
            if ( current_ptr->next[index] == NULL)
            {
                current_ptr->next[index] = malloc(sizeof(node));
                inittrie(current_ptr->next[index]);
            }
            current_ptr = current_ptr->next[index];
        }
        current_ptr->is_word = true;
    }

    fclose(dictptr);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    int count = 0;
    node* tmp = trie;
    for ( int i = 0; i < TRIEWIDTH; i++ )
    {
        if ( trie->next[i] != NULL )
        {
            trie = trie->next[i];
            count += size();
        }
        trie = tmp;
    }
    if ( trie->is_word )
        count++;
    return count;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    node* tmp = trie;
    for ( int i = 0; i < TRIEWIDTH; i++ )
    {
        if ( trie->next[i] != NULL )
        {
            trie = trie->next[i];
            unload();
        }
        trie = tmp;
    }
    free(trie);
    return true;
}
