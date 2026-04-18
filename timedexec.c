// 
//  timedexec.c
//  Tavish Chen
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>

typedef struct {
    pid_t child_pid;
    long  wall_sec, cpu_sec, mem_mb;
    volatile int done, kill_reason;
} MonState;

static double get_cpu(pid_t pid) {
    char path[64]; long u = 0, s = 0;
    snprintf(path, sizeof(path), "/proc/%d/stat", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    fscanf(f, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld", &u, &s);
    fclose(f);
    return (double)(u + s) / sysconf(_SC_CLK_TCK);
}

static long get_rss(pid_t pid) {
    char path[64], line[128];
    snprintf(path, sizeof(path), "/proc/%d/status", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    while (fgets(line, sizeof(line), f))
        if (strncmp(line, "VmRSS:", 6) == 0) { fclose(f); return atol(line + 6) / 1024; }
    fclose(f); return -1;
}

static void *monitor(void *arg) {
    MonState *st = arg;
    struct timespec start, now, ts = { 0, 500000000L };
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (!st->done) {
        nanosleep(&ts, NULL);
        clock_gettime(CLOCK_MONOTONIC, &now);
        long wall = now.tv_sec - start.tv_sec;

        if (st->wall_sec > 0 && wall >= st->wall_sec)
            { st->kill_reason = 1; kill(st->child_pid, SIGKILL); break; }
        if (st->cpu_sec  > 0 && get_cpu(st->child_pid) >= st->cpu_sec)
            { st->kill_reason = 2; kill(st->child_pid, SIGKILL); break; }
        if (st->mem_mb   > 0 && get_rss(st->child_pid) >= st->mem_mb)
            { st->kill_reason = 3; kill(st->child_pid, SIGKILL); break; }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int opt;
    MonState st = {0};

    while ((opt = getopt(argc, argv, "t:c:m:")) != -1) {
        switch (opt) {
            case 't': st.wall_sec = atol(optarg); break;
            case 'c': st.cpu_sec  = atol(optarg); break;
            case 'm': st.mem_mb   = atol(optarg); break;
            default:
                fprintf(stderr, "Usage: %s -t <sec> -c <sec> -m <MB> -- <command>\n", argv[0]);
                return 1;
        }
    }

    char **cmd = &argv[optind];
    if (*cmd == NULL) {
        fprintf(stderr, "Error: No command specified.\n");
        return 1;
    }

    struct timespec wall_start, wall_end;
    clock_gettime(CLOCK_MONOTONIC, &wall_start);

    pid_t child = fork();
    if (child == 0) { execvp(cmd[0], cmd); perror("execvp"); _exit(127); }

    st.child_pid = child;
    pthread_t tid;
    pthread_create(&tid, NULL, monitor, &st);

    int status;
    waitpid(child, &status, 0);
    st.done = 1;
    pthread_join(tid, NULL);

    clock_gettime(CLOCK_MONOTONIC, &wall_end);
    double elapsed = (wall_end.tv_sec - wall_start.tv_sec)
                   + (wall_end.tv_nsec - wall_start.tv_nsec) / 1e9;

    struct rusage ru;
    getrusage(RUSAGE_CHILDREN, &ru);

    printf("\n--- Performance Metrics ---\n");
    printf("Wall Time   : %.3f s\n", elapsed);
    printf("CPU (user)  : %.3f s\n", ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1e6);
    printf("CPU (sys)   : %.3f s\n", ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1e6);
    printf("Peak RSS    : %ld kB\n",  ru.ru_maxrss);

    const char *reasons[] = {"", "wall-clock limit", "CPU limit", "memory limit"};
    if (st.kill_reason)
        printf("Killed by   : %s\n", reasons[st.kill_reason]);

    return st.kill_reason == 1 ? 124 :
           st.kill_reason == 2 ? 125 :
           st.kill_reason == 3 ? 126 :
           WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}