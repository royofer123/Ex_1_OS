#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void execute_gpg(char *input_file, char *passphrase) {
    int gpg_pipe[2];
    pipe(gpg_pipe);

    pid_t gpg_pid = fork();
    if (gpg_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (gpg_pid == 0) {
        close(gpg_pipe[0]); // Close read end of gpg pipe
        dup2(gpg_pipe[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(gpg_pipe[1]); // Close write end of gpg pipe

        execlp("gpg", "gpg", "--decrypt", "--passphrase", passphrase, "-o", "-", input_file, NULL);
        perror("exec gpg");
        exit(EXIT_FAILURE);
    }

    close(gpg_pipe[1]); // Close write end of gpg pipe
    dup2(gpg_pipe[0], STDIN_FILENO); // Redirect stdin to read from gpg pipe
    close(gpg_pipe[0]); // Close read end of gpg pipe
}

void execute_gunzip() {
    int gunzip_pipe[2];
    pipe(gunzip_pipe);

    pid_t gunzip_pid = fork();
    if (gunzip_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (gunzip_pid == 0) {
        close(gunzip_pipe[0]); // Close read end of gunzip pipe
        dup2(gunzip_pipe[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(gunzip_pipe[1]); // Close write end of gunzip pipe

        execlp("gunzip", "gunzip", "-", NULL);
        perror("exec gunzip");
        exit(EXIT_FAILURE);
    }

    close(gunzip_pipe[1]); // Close write end of gunzip pipe
    dup2(gunzip_pipe[0], STDIN_FILENO); // Redirect stdin to read from gunzip pipe
    close(gunzip_pipe[0]); // Close read end of gunzip pipe
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file.gpg> <passphrase>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    execute_gpg(argv[1], argv[2]);
    execute_gunzip();

    execlp("tar", "tar", "xvf", "-", NULL);
    perror("exec tar");
    exit(EXIT_FAILURE);
}
