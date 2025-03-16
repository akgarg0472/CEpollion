#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "utils.h"
#include "command_handler.h"

#define SUCCESS_RESP_MSG "OK"
#define FAILURE_RESP_MSG "FAILED"
#define INVALID_KEY "MISSING_KEY"
#define INVALID_ARGS "MISSING_ARG"
#define INVALID_CMD_MSG "INVALID_COMMAND"

#define DEFAULT_HASHMAP_SIZE 1024 /** Default size for the hash map */
#define RESP_BUFF_SIZE 256        /** Size of the response buffer */

HashMap *map = NULL;

/**
 * @brief Initializes the command handler.
 *
 * This function ensures that the global hashmap data structure is created before handling commands.
 */
void initialize_command_handler()
{
    if (!map)
    {
        map = create_hash_map(DEFAULT_HASHMAP_SIZE);
    }
}

/**
 * @brief Executes a given command and returns a response.
 *
 * This function processes the command based on its type and performs actions such as
 * setting, getting, removing, or retrieving all key-value pairs from the hashmap.
 *
 * @param cmd Pointer to a Command struct containing the parsed command.
 * @return A dynamically allocated response string. Caller must free it when done.
 */
char *execute_command(Command *cmd)
{
    char *response = malloc(RESP_BUFF_SIZE);

    if (!response)
    {
        return FAILURE_RESP_MSG;
    }

    memset(response, 0, RESP_BUFF_SIZE);

    switch (cmd->type)
    {
    case CMD_SET:
        if (!cmd->key)
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", INVALID_KEY);
        }
        else if (!cmd->args || !cmd->args[0])
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", INVALID_ARGS);
        }
        else
        {
            remove_trailing_newline(cmd->key);
            remove_trailing_newline(cmd->args[0]);
            bool success = hash_map_set(map, cmd->key, cmd->args[0]);

            if (success)
            {
                snprintf(response, RESP_BUFF_SIZE, "%s\n", SUCCESS_RESP_MSG);
            }
            else
            {
                snprintf(response, RESP_BUFF_SIZE, "%s\n", FAILURE_RESP_MSG);
            }
        }
        break;

    case CMD_GET:
        if (!cmd->key)
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", INVALID_KEY);
        }
        else
        {
            remove_trailing_newline(cmd->key);
            char *value = hash_map_get(map, cmd->key);

            if (value)
            {
                snprintf(response, RESP_BUFF_SIZE, "%s\n", value);
            }
            else
            {
                snprintf(response, RESP_BUFF_SIZE, "%p\n", NULL);
            }
        }
        break;

    case CMD_REMOVE:
        if (!cmd->key)
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", INVALID_KEY);
        }
        else
        {
            remove_trailing_newline(cmd->key);
            bool success = hash_map_remove(map, cmd->key);
            snprintf(response, RESP_BUFF_SIZE, "%d\n", success);
        }
        break;

    case CMD_GET_ALL:
        char *all_entries = hash_map_get_all(map);
        if (!all_entries)
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", FAILURE_RESP_MSG);
            break;
        }

        size_t len = strlen(all_entries) + 2;
        char *large_response = malloc(len);
        if (!large_response)
        {
            snprintf(response, RESP_BUFF_SIZE, "%s\n", FAILURE_RESP_MSG);
        }
        else
        {
            snprintf(large_response, len, "%s\n", all_entries);
            free(response);
            free(all_entries);
            return large_response;
        }
        break;

    default:
        snprintf(response, RESP_BUFF_SIZE, "%s\n", INVALID_CMD_MSG);
        break;
    }

    return response;
}
