#pragma once

#include <stdint.h>

size_t get_arg_count(char *input, size_t size);
char **get_args(char *input, size_t size, size_t arg_count);
char *get_command(char *input, size_t size);
char *check_path(char *cmd, char *path);
char *get_full_command(char *cmd);

