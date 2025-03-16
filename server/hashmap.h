#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Structure representing a key-value pair in the hashmap.
 *
 * This structure stores a key and its associated value. It also supports
 * chaining via a linked list to handle collisions.
 */
typedef struct KVPair
{
    char *key;           /** The key string (dynamically allocated) */
    char *value;         /** The value string (dynamically allocated) */
    struct KVPair *next; /** Pointer to the next key-value pair (for collision handling) */
} KVPair;

/**
 * @brief Structure representing the HashMap.
 *
 * The hashmap uses an array of buckets where each bucket is a linked list
 * of key-value pairs. The `capacity` defines the total number of buckets,
 * while `size` tracks the number of key-value pairs stored.
 */
typedef struct
{
    size_t capacity;  /** Number of buckets in the hashmap */
    size_t size;      /** Current number of key-value pairs stored */
    KVPair **buckets; /** Array of bucket pointers */
} HashMap;

/**
 * @brief Creates a new hashmap with the specified capacity.
 *
 * Allocates memory for a hashmap and initializes its internal structures.
 *
 * @param capacity The initial number of buckets.
 * @return Pointer to the newly created HashMap or NULL if allocation fails.
 */
HashMap *create_hash_map(size_t capacity);

/**
 * @brief Inserts or updates a key-value pair in the hashmap.
 *
 * If the key already exists, its value is updated. Otherwise, a new key-value
 * pair is inserted.
 *
 * @param map Pointer to the HashMap.
 * @param key The key string (must be null-terminated).
 * @param value The value string (must be null-terminated).
 * @return True if insertion/update is successful, false otherwise.
 */
bool hash_map_set(HashMap *map, const char *key, const char *value);

/**
 * @brief Retrieves the value associated with a key in the hashmap.
 *
 * @param map Pointer to the HashMap.
 * @param key The key string to search for.
 * @return Pointer to the value string if found, or NULL if the key does not exist.
 *         The returned string should NOT be freed by the caller.
 */
char *hash_map_get(HashMap *map, const char *key);

/**
 * @brief Removes a key-value pair from the hashmap.
 *
 * @param map Pointer to the HashMap.
 * @param key The key string to remove.
 * @return True if the key was successfully removed, false if the key was not found.
 */
bool hash_map_remove(HashMap *map, const char *key);

/**
 * @brief Retrieves all key-value pairs as a formatted string.
 *
 * The returned string contains all key-value pairs in a human-readable format.
 * The caller is responsible for freeing the returned string.
 *
 * @param map Pointer to the HashMap.
 * @return A dynamically allocated string containing all key-value pairs, or NULL if empty.
 */
char *hash_map_get_all(HashMap *map);

/**
 * @brief Frees all memory associated with the hashmap.
 *
 * This function deallocates all key-value pairs, buckets, and the hashmap itself.
 *
 * @param map Pointer to the HashMap to be freed.
 */
void free_hash_map(HashMap *map);

#endif // HASHMAP_H
