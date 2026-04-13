#include <iostream>
#include <cstdlib>
#include <csignal>
#include <stdexcept>

#include <sys/types.h>
#include <signal.h>

int main() {
    // Run ps to find the PID of ./server
    FILE* pipe = popen("ps -eo pid,comm | awk '$2 == \"server\" {print $1}'", "r");
    if (!pipe) {
        std::cerr << "Error: failed to run ps\n";
        return 1;
    }

    pid_t pid = 0;
    fscanf(pipe, "%d", &pid);
    pclose(pipe);

    if (pid <= 0) {
        std::cerr << "Error: could not find running process './server'\n";
        return 1;
    }

    // Output the command as if it was run in the terminal
    std::cout << "kill " << pid << " -" << SIGKILL << '\n';

    if (kill(pid, SIGKILL) != 0) {
        perror("kill");
        return 1;
    }

    return 0;
}