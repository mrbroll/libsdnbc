#include "../headers/dictionary.h"
#include <stdlib.h>
#include <string.h>

#ifndef MAXHASH
    #define MAXHASH 64
#endif

typedef struct dictionaryEntry_s dictionaryEntry_t;

struct dictionaryEntry_s
{
    char *key;
    dictionaryEntry_t *next;
    void *data;
};

unsigned int getHash(const char *key)
{
    unsigned int hash;
    for (hash = 0; *key != '\0'; key++)
        hash += *key + (hash << 5);

    return hash % MAXHASH;
}

dictionaryEntry_t *createEntry(char *key, void *data, size_t dataSize)
{
    if (!(key && data && dataSize)) //bad param(s)
        return NULL;

    dictionaryEntry_t *dEnt = malloc(sizeof(dictionaryEntry_t));
    dEnt->key = (char *)malloc(strlen(key) + 1);
    strcpy(dEnt->key, key);
    dEnt->next = NULL;
    dEnt->data = malloc(dataSize);
    memcpy(dEnt->data, data, dataSize);
    return dEnt;
}

dictionaryEntry_t *destroyEntry(dictionaryEntry_t *dEnt)
{
    if (!dEnt)
        return NULL;

    free(dEnt->data);
    dEnt->data = NULL;
    free(dEnt->key);
    dEnt->key = NULL;
    return dEnt->next;
}

sdnb_dictionary_t *sdnb_dictionary_create(void)
{
    sdnb_dictionary_t *newDict = (sdnb_dictionary_t *)malloc(sizeof(sdnb_dictionary_t));
    if (!newDict)
        return NULL;

    newDict->numEntries = 0;
    newDict->entries = (dictionaryEntry_t **)malloc(MAXHASH * sizeof(dictionaryEntry_t *));
    if (!newDict->entries)
        return NULL;

    return newDict;
}

void *sdnb_dictionary_get(sdnb_dictionary_t *dict, const char *key)
{
    if (!(dict || key))
        return NULL;

    dictionaryEntry_t *dEnt = ((dictionaryEntry_t **)dict->entries)[getHash(key)];
    if (!dEnt)
        return NULL;

    do {
        if (!strcmp(dEnt->key, key))
            return (void *)dEnt->data;
    } while ((dEnt = dEnt->next));

    return NULL;
}

int sdnb_dictionary_set(sdnb_dictionary_t *dict, char *key, void *data, size_t dataSize)
{
    if (!dict || sdnb_dictionary_get(dict, key))
        return 1;
    
    dictionaryEntry_t *dEnt = ((dictionaryEntry_t **)dict->entries)[getHash(key)];

    if (!dEnt) {
        ((dictionaryEntry_t **)dict->entries)[getHash(key)] = createEntry(key, data, dataSize);
        return 0;
    }

    do {
        if (!dEnt->next) {
            dEnt->next = createEntry(key, data, dataSize);
            return 0;
        }
    } while ((dEnt = dEnt->next));

    //we shouldn't ever get here, but the compiler won't know that
    return 1;
}

void sdnb_dictionary_remove(sdnb_dictionary_t *dict, char *key)
{
    if (!dict)
        return;

    dictionaryEntry_t *dEnt = ((dictionaryEntry_t **)dict->entries)[getHash(key)];

    if (!dEnt)
        return;

    if (!strcmp(dEnt->key, key)) {
        ((dictionaryEntry_t **)dict->entries)[getHash(key)] = dEnt->next;
        destroyEntry(dEnt);
        return;
    }

    dictionaryEntry_t *prev = dEnt;
    while ((dEnt = dEnt->next)) {
        if (!strcmp(dEnt->key, key)) {
            prev->next = destroyEntry(dEnt);
            return;
        }
        prev = dEnt;
    }
}

void sdnb_dictionary_destroy(sdnb_dictionary_t *dict)
{
    if (!dict)
        return;

    dictionaryEntry_t **dEntries = (dictionaryEntry_t **)dict->entries;
    if (dEntries) {
        dictionaryEntry_t *dEnt;
        int i;
        for (int i = 0; i < MAXHASH; i++) {
            dEnt = dEntries[i];
            if (!dEnt)
                continue;
            while ((dEnt = destroyEntry(dEnt)));
        }
        free(dict->entries);
        dict->entries = NULL;
    }

    dict->numEntries = 0;
    free(dict);
    dict = NULL; 
}
