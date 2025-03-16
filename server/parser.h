#ifndef PARSER_H
#define PARSER_H

#include "hashmap.h"

/**
 * @brief Represents different types of client commands.
 */
typedef enum
{
    CMD_INVALID = -1, /**< Invalid or unrecognized command */
    CMD_SET,          /**< Set a key-value pair */
    CMD_GET,          /**< Retrieve a value by key */
    CMD_REMOVE,       /**< Remove a key-value pair */
    CMD_GET_ALL       /**< Retrieve all stored key-value pairs */
} CommandType;

/**
 * @brief Structure representing a parsed command.
 */
typedef struct
{
    CommandType type; /**< Type of command */
    char *key;        /**< Key associated with the command (if applicable) */
    char **args;      /**< Additional arguments (if any) */
} Command;

/**
 * @brief Parses client input and populates a Command struct.
 *
 * @param input The raw input string from the client.
 * @param cmd Pointer to a Command struct to store the parsed command.
 */
void parse_client_input(const char *input, Command *cmd);

#endif // PARSER_H
