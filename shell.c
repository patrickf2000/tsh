#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>

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
    char *full_path = (char *)malloc(sizeof(char)*len);
    
    strcat(full_path, path);
    strcat(full_path, cmd);
    
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

void run(char *cmd, char **args)
{
    pid_t pid = fork();
    
    if (pid < 0)
    {
        printf("Unable to fork.\n");
        return;
    }
    else if (pid == 0)
    {
        // Child
        execv(cmd, args);
        
        // If we reach this point, an error occurred
        printf("Unable to run command.\n");
        exit(1);
    }
    else
    {
        int wstatus;
        waitpid(-1, &wstatus, 0);
    }
}

void execute_command(char *input, int size)
{
    char *cmd = get_command(input, size);
    char *full_cmd = get_full_command(cmd);
    
    if (full_cmd == NULL)
    {
        printf("Unknown command.\n");
        return;
    }
    
    size_t arg_count = get_arg_count(input, size);
    char **args = get_args(input, size, arg_count);
    
    run(full_cmd, args);
    
    // Free everything
    free(cmd);
    free(full_cmd);
    
    for (int i = 0; i<arg_count - 1; i++)
        if (args[i]) free(args[i]);
    free(args);
}

void get_line(char *input, int size)
{
    // Set the input to raw mode
    static struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    
    // Clear the buffer
    char old_buf[100];
    
    for (int i = 0; i < size; i++) {
        old_buf[i] = input[i];
        input[i] = 0;
    }
        
    unsigned char c = 0;
    int pos = 0;
    int history = 0;
    int original_size = strlen(old_buf);
    
    do {
        c = getchar();
        
        if ((unsigned char)c == 27) {
            char c1 = c;
            char c2 = getchar();
            c = getchar();
            if (c == 'A') {
                if (original_size == 0)
                    continue;
                
                memcpy(input, old_buf, original_size);
                pos += original_size;
                
                printf("%s", old_buf);
            } else {
                putchar(c);
                putchar(c1);
                putchar(c2);
            }
        } else if (c == 0 || c == '\n') {
            putchar('\n');
            break;
        } else {
            input[pos] = c;
            ++pos;
            
            putchar(c);
        }
    } while (pos < size);
    
    input[pos] = 0;
    
    // Restore terminal to normal mode
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

void run_shell()
{
    char input_buf[100];
    
    for (;;) {
        printf("> ");
        get_line(input_buf, 100);
        
        if (strcmp(input_buf, "exit") == 0) {
            break;
        }
        
        execute_command(input_buf, 100);
    }
}

int main(int argc, char **argv)
{
    run_shell();
    return 0;
}

