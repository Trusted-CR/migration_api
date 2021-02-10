#include "trusted_cr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>

static bool __migrated_to_sw = false;

void trusted_cr_migrate_to_sw() {
    if(__migrated_to_sw) {
        printf("Error: Request to migrate to the SW but already executing in SW\n");
        return;
    }

    printf("Going to migrate to the SW\n");

    pid_t parent_pid = getpid();

    // Fork and execute trusted_cr which will dump the parent_pid with CRIU
    // after that it will migrate the checkpoint to the secure world.
    pid_t pid = fork();
    if(pid == -1)
        exit(-1);
    if(pid == 0) {
        // Prepare the arguments for CRIU
        char pid_buffer[10] = { };
        snprintf(pid_buffer, 10, "%d", parent_pid);

        char *args[]={"./trusted_cr", "-m", pid_buffer, NULL}; 
        
        // Detach from the parent process
        setsid();

        // File descriptors are inherited from the parent even after exec, close them to be sure.
        for (int fd = 3; fd < 256; fd++) 
            (void) close(fd);
        
        // We are double forking, otherwise CRIU will try to dump itself. Even though we call setsid()
        // linux will still list the forked process as a child in /proc/<pid>/tasks/<pid>/children.
        // CRIU uses the 'children' file to dump a process and its children. By double forking and exiting
        // the first fork, it is no longer a child.
        pid_t double_fork = fork();
        switch (double_fork) {
            case -1: /* error */
                exit(-1);
            case 0:  /* child */
                execvp(args[0], args);
        }

        // Let the child terminate so the parent process has no children.
        exit(0);
    }

    // Wait for the child process to die, otherwise it is still a child according to
    // /proc/*pid*/task/*pid*/children and therefore it will be checkpointed by CRIU
    waitpid(pid, 0, 0);

    // First we store 0x1 in w0
    // Next we keep looping until w0 becomes 0x0 which is never... right..?
    // We change the value with the debugger (CRIU) to 0x0 and checkpoint the binary.
    // In this way the first instruction in the secure world will be after the
    // migration function. 
    __asm__("   mov     w1,#0x1\n\t"
            "__PROTECTION_LOOP_ENTER_SW:\n\t"
            "   mov    w8,#0x22\n\t"    // Store the pause syscall number.
            "   svc    0x0\n\t"         // Perform the system call.
            "   cmp     w1,#0x0\n\t"
            "   b.ne    __PROTECTION_LOOP_ENTER_SW\n\t");

    __migrated_to_sw = true;

	printf("Succesfully migrated to the SW!\n");
}


void trusted_cr_migrate_back_to_nw() {
    if(!__migrated_to_sw) {
        printf("Error: Request to migrate back to NW but already executing in NW\n");
        return;
    }

    printf("Going to migrate to the NW\n");

    // To migrate back to the normal world we again use a never ending loop.
    // Only when register w1 becomes 0x0 the loop stops. Before we enter the never
    // ending loop we perform syscall 1000. This system call does not exist but we
    // detect it in OP-TEE as an indicator that the program wants to migrate back.
    // OP-TEE detects it and changes register w1 to 0x0 before migrating back so 
    // that the program will exit the loop. 
    __asm__("   mov    w1,#0x1\n\t"
            "   mov    w8,#0x3e8\n\t"   // Store syscall number 1000 (0x3eb).
            "   svc    0x0\n\t"         // Perform the system call.
            "__PROTECTION_LOOP_EXIT_SW:\n\t"
            "   cmp    w1,#0x0\n\t"
            "   b.ne   __PROTECTION_LOOP_EXIT_SW\n\t");

    __migrated_to_sw = false;

	printf("Succesfully migrated back to the NW!\n");
}
