#ifndef MIGRATION_H
#define MIGRATION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void migrate_to_sw() {
    printf("Going to migrate to the SW\n");

    pid_t parent_pid = getpid();

    printf("parent_pid: %d\n", parent_pid);

    // char *aargv[3];
    // aargv[0] = "../criu.sh";
    // aargv[1] = "dump -t -D check --shell-job #-v0;";
    // aargv[2] = 0;

    pid_t pid = fork();
    switch (pid) {
        case -1: /* error */
            exit(-1);
        case 0:  /* child */
            // Detach from the parent process otherwise the child will be checkpointed as well
            setsid();

            // File descriptors are inherited from the parent even after exec, close them to be sure.
            for (int fd = 3; fd < 256; fd++) 
                (void) close(fd);
            
            printf("my pid: %d and the parent is: %d\n", getpid(), parent_pid);
            exit(0);
            //execvp("../criu", aargv);
    }

    sleep(2);
    exit(0);
    raise(SIGSTOP);
}


void migrate_back_to_nw() {
    printf("Going to migrate to the NW\n");
}

#endif // MIGRATION_H