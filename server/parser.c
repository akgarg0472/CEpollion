#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define COMMAND_MAX_SIZE 16 /**Maximum possible length of a command string */

/**
 * @brief Converts a command string to its corresponding CommandType.
 *
 * This function converts the given command string to uppercase,
 * then matches it against known commands (`SET`, `GET`, `DEL`, `GETALL`).
 * If the command is not recognized, it returns `CMD_INVALID`.
 *
 * @param str The command string to convert.
 * @return The corresponding CommandType, or CMD_INVALID if unrecognized.
 */
CommandType string_to_command(const char *str)
{
    char command_str[COMMAND_MAX_SIZE] = {0};

    for (int i = 0; i < COMMAND_MAX_SIZE - 1 && str[i]; i++)
    {
        if (str[i] == '\n')
            break;
        command_str[i] = toupper((unsigned char)str[i]);
    }

    if (strcmp(command_str, "SET") == 0)
    {
        return CMD_SET;
    }
    else if (strcmp(command_str, "GET") == 0)
    {
        return CMD_GET;
    }
    else if (strcmp(command_str, "DEL") == 0)
    {
        return CMD_REMOVE;
    }
    else if (strcmp(command_str, "GETALL") == 0)
    {
        return CMD_GET_ALL;
    }
    else
    {
        return CMD_INVALID;
    }
}

/**
 * @brief Parses the client input string and populates a Command struct.
 *
 * This function extracts the command type, key, and optional arguments
 * from the given input string. It tokenizes the input, determines the
 * command type, and stores arguments in a dynamically allocated array.
 *
 * @param input The raw input string from the client.
 * @param cmd Pointer to a Command struct to store the parsed command.
 */
void parse_client_input(const char *input, Command *cmd)
{
    while (isspace((unsigned char)*input))
    {
        input++;
    }

    char *command_str = strtok((char *)input, " ");
    if (!command_str)
    {
        cmd->type = CMD_INVALID;
        return;
    }

    cmd->type = string_to_command(command_str);
    if (cmd->type == CMD_INVALID)
    {
        return;
    }

    cmd->key = strtok(NULL, " ");
    if (cmd->key)
    {
        cmd->args = malloc(sizeof(char *) * 10);
        int i = 0;

        while (cmd->key && (cmd->args[i] = strtok(NULL, " ")) != NULL)
        {
            i++;
        }
        cmd->args[i] = NULL;
    }
}
