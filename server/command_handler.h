#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "parser.h"
#include "logger.h"

/**
 * @brief Initializes the command handler.
 *
 * This function sets up any necessary resources for handling commands.
 * It should be called before executing any commands.
 */
void initialize_command_handler();

/**
 * @brief Executes a given command and returns the response.
 *
 * This function takes a parsed command, processes it, and returns a response.
 * The caller is responsible for freeing the returned string if needed.
 *
 * @param cmd Pointer to a Command struct containing the parsed command.
 * @return A dynamically allocated string containing the command response.
 */
char *execute_command(Command *cmd);

#endif // COMMAND_HANDLER_H
