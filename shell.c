#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>

#include "parser.h"

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
    // Check built-ins
    char *cmd = get_command(input, size);
    
    if (strcmp(cmd, "cd") == 0) {
        run_cd(input, size);
        
        free(cmd);
        return;
    }
    
    // Otherwise, check the system
    char *full_cmd = get_full_command(cmd);
    
    if (full_cmd == NULL)
    {
        printf("Unknown command.\n");
        
        free(cmd);
        free(full_cmd);
        return;
    }
    
    // Get arguments list and run
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
    char *input_buf = (char *)malloc(sizeof(char)*100);
    
    for (;;) {
        printf("> ");
        get_line(input_buf, 100);
        
        if (strcmp(input_buf, "exit") == 0) {
            break;
        }
        
        execute_command(input_buf, 100);
    }
    
    if (input_buf)
        free(input_buf);
}

int main(int argc, char **argv)
{
    run_shell();
    return 0;
}

