
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

