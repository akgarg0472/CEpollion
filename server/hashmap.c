#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashmap.h"

#define BUFFER_SIZE 1024

/**
 * @brief A very basic hash function that sums ASCII values of characters.
 * @param key The input key (string).
 * @param capacity The total number of buckets in the hash map.
 * @return Hash index (0 to capacity-1).
 */
unsigned int hash(const char *key, size_t capacity)
{
    unsigned int hash_value = 0;
    while (*key)
    {
        hash_value += (unsigned char)(*key);
        key++;
    }
    return hash_value % capacity;
}

/**
 * @brief Creates and initializes a new hash map.
 * @param capacity The total number of buckets in the hash map.
 * @return Pointer to the newly allocated HashMap structure.
 */
HashMap *create_hash_map(size_t capacity)
{
    HashMap *map = malloc(sizeof(HashMap));

    if (!map)
        return NULL;

    map->capacity = capacity;
    map->size = 0;
    map->buckets = calloc(capacity, sizeof(KVPair *));

    if (!map->buckets)
    {
        free(map);
        return NULL;
    }

    return map;
}

/**
 * @brief Inserts or updates a key-value pair in the hash map.
 * @param map Pointer to the HashMap structure.
 * @param key The key (string).
 * @param value The value (string).
 */
bool hash_map_set(HashMap *map, const char *key, const char *value)
{
    unsigned int index = hash(key, map->capacity);
    KVPair *entry = map->buckets[index];

    // Check if key already exists and update its value
    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            free(entry->value);
            entry->value = strdup(value);
            return true;
        }
        entry = entry->next;
    }

    // Insert new key-value pair at head of the linked list
    KVPair *new_pair = malloc(sizeof(KVPair));
    new_pair->key = strdup(key);
    new_pair->value = strdup(value);
    new_pair->next = map->buckets[index];
    map->buckets[index] = new_pair;
    map->size++;

    return true;
}

/**
 * @brief Retrieves the value associated with a given key.
 * @param map Pointer to the HashMap structure.
 * @param key The key (string).
 * @return The corresponding value (string), or NULL if key not found.
 */
char *hash_map_get(HashMap *map, const char *key)
{
    if (map->size == 0)
    {
        return NULL;
    }

    unsigned int index = hash(key, map->capacity);
    KVPair *entry = map->buckets[index];

    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

/**
 * @brief Removes a key-value pair from the hash map.
 * @param map Pointer to the HashMap structure.
 * @param key The key to remove.
 * @return true if key was removed, false if key was not found.
 */
bool hash_map_remove(HashMap *map, const char *key)
{
    unsigned int index = hash(key, map->capacity);
    KVPair *entry = map->buckets[index];
    KVPair *prev = NULL;

    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            if (prev)
            {
                prev->next = entry->next;
            }
            else
            {
                map->buckets[index] = entry->next;
            }

            free(entry->key);
            free(entry->value);
            free(entry);
            map->size--;
            return true;
        }

        prev = entry;
        entry = entry->next;
    }

    return false;
}

/**
 * @brief Retrieves all key-value pairs as a JSON-formatted string.
 * @param map Pointer to the HashMap structure.
 * @return Dynamically allocated JSON string. The caller must free() it.
 */
char *hash_map_get_all(HashMap *map)
{
    size_t buffer_size = BUFFER_SIZE;
    char *result = malloc(buffer_size);
    if (!result)
        return NULL;

    size_t pos = 0;
    result[pos++] = '{';

    bool has_entries = false;

    for (size_t i = 0; i < map->capacity; i++)
    {
        KVPair *entry = map->buckets[i];

        while (entry)
        {
            size_t required_size = pos + strlen(entry->key) + strlen(entry->value) + 6;

            if (required_size >= buffer_size)
            {
                buffer_size *= 2;
                char *new_result = realloc(result, buffer_size);
                if (!new_result)
                {
                    free(result);
                    return NULL;
                }
                result = new_result;
            }

            pos += snprintf(result + pos, buffer_size - pos, "\"%s\":\"%s\",", entry->key, entry->value);
            entry = entry->next;
            has_entries = true;
        }
    }

    if (has_entries)
    {
        result[pos - 1] = '}';
    }
    else
    {
        result[pos++] = '}';
    }
    result[pos] = '\0';

    return result;
}

/**
 * @brief Frees all memory allocated for the hash map.
 * @param map Pointer to the HashMap structure.
 */
void free_hash_map(HashMap *map)
{
    for (size_t i = 0; i < map->capacity; i++)
    {
        KVPair *entry = map->buckets[i];
        while (entry)
        {
            KVPair *temp = entry;
            entry = entry->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}
