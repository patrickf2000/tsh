
// builtin.c
// This handles various shell built-in utilities, such as the current working
// directory, environment variables, and so forth.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

// Checks an environment variable
size_t check_env(char *buf)
{
    char *env = getenv(buf);
    
    if (env == NULL) {
        return strlen(buf);
    }
    
    size_t len = strlen(env);
    
    for (int i = 0; i<strlen(buf); i++)
        buf[i] = 0;
    
    strcpy(buf, env);
}

// Run the CD built-in
void run_cd(char *input, int size)
{
    char *cd_path = (char *)malloc(sizeof(char)*100);
    int found = 0;
    int pos = 0;
    int is_env = 0;
    
    for (int i = 0; i<size; i++)
    {
        if (input[i] == ' ' && !found)
        {
            found = 1;
        }
        else if (found)
        {
            if (input[i] == '$') {
                is_env = 1;
                continue;
            }
            
            cd_path[pos] = input[i];
            ++pos;
        }
    }
    
    cd_path[pos] = 0;
    
    if (is_env)
    {
        check_env(cd_path);
    }
    
    if (chdir(cd_path) == -1)
    {
        printf("Unknown path\n");
        return;
    }
    
    free(cd_path);
}

// Run the SET built-in (sets environment variables)
void run_set(char *input, int size)
{
    int len = size - 4;
    char var_name[len];
    char var_value[len];
    
    int in_value = 0;
    int pos = 0;
    
    for (int i = 4; i<size; i++)
    {
        if (input[i] == '=' && !in_value)
        {
            in_value = 1;
            
            var_name[pos] = 0;
            pos = 0;
        }
        else if (in_value)
        {
            var_value[pos] = input[i];
            ++pos;
        }
        else
        {
            var_name[pos] = input[i];
            ++pos;
        }
    }
    
    var_value[pos] = 0;
    
    setenv(var_name, var_value, 1);
}

