#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

// Parse out the arguments
size_t get_arg_count(char *input, size_t size)
{
    size_t arg_count = 2;
    
    for (int i = 0; i < size; i++) {
        if (input[i] == ' ') {
            ++arg_count;
        }
    }
    
    return arg_count;
}

char **get_args(char *input, size_t size, size_t arg_count)
{
    char **args = (char **)malloc(sizeof(char *)*arg_count);
    args[arg_count - 1] = NULL;
    
    int arg_pos = 0;
    int buf_pos = 0;
    char *buf = (char *)malloc(sizeof(char) * 100);
    
    for (int i = 0; i<size; i++) {
        if (input[i] == ' ') {
            buf_pos += 1;
            args[arg_pos] = (char *)malloc(sizeof(char) * buf_pos);
            strncpy(args[arg_pos], buf, buf_pos);
            args[arg_pos][buf_pos - 1] = 0;
            
            for (int i = 0; i<buf_pos; i++)
                buf[i] = 0;
                
            buf_pos = 0;
            arg_pos += 1;
        } else {
            buf[buf_pos] = input[i];
            ++buf_pos;
        }
    }
    
    buf_pos += 1;
    args[arg_pos] = (char *)malloc(sizeof(char) * buf_pos);
    strncpy(args[arg_pos], buf, buf_pos);
    args[arg_pos][buf_pos - 1] = 0;
    
    free(buf);
    return args;
}

// Parse out the command
char *get_command(char *input, size_t size)
{
    size_t cmd_length = 0;
    
    for (int i = 0; i < size; i++) {
        if (input[i] == ' ') {
            break;
        }
        ++cmd_length;
    }
    
    char *cmd = (char *)malloc(sizeof(char)* (cmd_length + 1));
    
    for (int i = 0; i < cmd_length; i++)
        cmd[i] = input[i];
        
    cmd[cmd_length] = 0;
        
    return cmd;
}

// Checks the path variable to get the full command
char *check_path(char *cmd, char *path) {
    int len = strlen(cmd) + strlen(path) + 1;
    char *full_path = (char *)malloc(sizeof(char)*150);
    
    strcat(full_path, path);
    strcat(full_path, cmd);
    full_path[len-1] = 0;
    
    if (access(full_path, X_OK) == 0) {
        return full_path;
    }
    
    return NULL;
}

char *get_full_command(char *cmd) {
    int cmd_len = strlen(cmd);
    
    char *path = getenv("PATH");
    int path_len = strlen(path);
    
    char *path_buf = (char *)malloc(sizeof(char)*100);
    int pos = 0;
    
    char *full_path = NULL;
    
    for (int i = 0; i<=path_len; i++) {
        if (path[i] == 0 || path[i] == ':') {
            path_buf[pos] = '/';
            path_buf[pos + 1] = 0;
            
            full_path = check_path(cmd, path_buf);
            if (full_path) {
                break;
            } else {
                free(full_path);
                full_path = NULL;
            }
            
            for (int i = 0; i<pos; i++)
                path_buf[pos] = 0;
                
            pos = 0;
        } else {
            path_buf[pos] = path[i];
            ++pos;
        }
    }
    
    free(path_buf);
    return full_path;
}

