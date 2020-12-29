#ifndef MIGRATION_H
#define MIGRATION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void migrate_to_sw() {
    printf("Going to migrate to the SW\n");

    pid_t parent_pid = getpid();

    printf("parent_pid: %d\n", parent_pid);

    pid_t pid = fork();
    if(pid == -1)
        exit(-1);
    if(pid == 0) {
        // Prepare the arguments for CRIU
        char pid_buffer[10] = { };
        snprintf(pid_buffer, 10, "%d", parent_pid);

        char *args[]={"./criu.sh", "dump", "-t", pid_buffer, "-D", "check", "--shell-job", "-v4", NULL}; 
        
        // Detach from the parent process
        setsid();

        // File descriptors are inherited from the parent even after exec, close them to be sure.
        for (int fd = 3; fd < 256; fd++) 
            (void) close(fd);
        
        pid_t double_fork = fork();
        switch (double_fork) {
            case -1: /* error */
                exit(-1);
            case 0:  /* child */
                execvp("./criu.sh", args);
        }

        // Let the child terminate so the parent process has no children.
        exit(0);
    }

    // Wait for the child process to die, otherwise it is still a child according to
    // /proc/*pid*/task/*pid*/children and therefore it will be checkpointed by CRIU
    waitpid(pid, 0, 0);

    // Let CRIU do the SIGCONT to restore execution
    
    raise(SIGSTOP);
}


void migrate_back_to_nw() {
    printf("Going to migrate to the NW\n");
}

#endif // MIGRATION_H