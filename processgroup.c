#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mman.h>   
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>    
#include <time.h>

typedef struct {
    volatile int stop;      
    volatile int count;      
} SharedState;

int is_number(const char *str) {
    for (int i = 0; str[i] != '\0'; i++)
        if (!isdigit(str[i])) return 0;
    return 1;
}

int get_pgid_from_proc(int pid) {
    char path[64];
    int pgid = -1;

    // Cygwin and Linux both provide /proc/[pid]/stat
    // The 5th value in this file is the Process Group ID (pgrp)
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    // We skip the first 4 fields: 1(pid) 2(comm) 3(state) 4(ppid)
    // and store the 5th: 5(pgrp)
    if (fscanf(f, "%*d %*s %*c %*d %d", &pgid) != 1) {
        pgid = -1;
    }

    fclose(f);
    return pgid;
}


/*
int get_pgid_from_proc(int pid) {
    char path[64], line[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    int pgid = -1;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "NSpgid:", 7) == 0) {
            pgid = atoi(line + 7);
            break;
        }
    }
    fclose(f);
    return pgid;
}
*/

void print_process_info(int pid) {
    char path[64], line[256];
    char name[128] = "unknown";
    long vmrss = -1;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("  PID %-6d  [process exited during scan]\n", pid);
        return;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "Name:", 5) == 0)  sscanf(line + 5, "%s", name);
        if (strncmp(line, "VmRSS:", 6) == 0) vmrss = atol(line + 6);
    }
    fclose(f);

    if (vmrss >= 0)
        printf("  PID %-6d  Name: %-20s  Memory (RSS): %ld kB\n", pid, name, vmrss);
    else
        printf("  PID %-6d  Name: %-20s  Memory: N/A\n", pid, name);
}

int list_processes_in_group(int target_pgid) {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) { perror("opendir /proc"); return -1; }

    struct dirent *entry;
    int count = 0;

    printf("\nProcesses in group %d:\n", target_pgid);
    printf("  %-10s  %-24s  %s\n", "PID", "Name", "Memory");
    printf("  %s\n", "------------------------------------------------------");

    while ((entry = readdir(proc_dir)) != NULL) {
        if (!is_number(entry->d_name)) continue;
        int pid = atoi(entry->d_name);
        if (get_pgid_from_proc(pid) == target_pgid) {
            print_process_info(pid);
            count++;
        }
    }

    closedir(proc_dir);

    if (count == 0)
        printf("  (no processes found in group %d)\n", target_pgid);

    printf("\nTotal: %d process(es) in group %d\n", count, target_pgid);
    return count;
}

typedef struct {
    int pgid;
    int interval_sec;
    SharedState *state;  
} MonitorArgs;


void *monitor_thread(void *arg) {
    MonitorArgs *m = (MonitorArgs *)arg;

    while (!m->state->stop) {
        DIR *proc_dir = opendir("/proc");
        if (!proc_dir) break;

        struct dirent *entry;
        int count = 0;

        while ((entry = readdir(proc_dir)) != NULL) {
            if (!is_number(entry->d_name)) continue;
            int pid = atoi(entry->d_name);
            if (get_pgid_from_proc(pid) == m->pgid) count++;
        }
        closedir(proc_dir);

       
        m->state->count = count;

        printf("[monitor] Group %d -> %d process(es) alive\n", m->pgid, count);
        fflush(stdout);

        if (count == 0) break;

        sleep(m->interval_sec);
    }

    return NULL;
}




int main(int argc, char *argv[]) {
    int opt;
    int target_pgid = -1;
    int signal_num  = -1;
    int watch_sec   = -1;

    while ((opt = getopt(argc, argv, "g:s:w:")) != -1) {
        switch (opt) {
            case 'g': target_pgid = atoi(optarg); break;
            case 's': signal_num  = atoi(optarg); break;
            case 'w': watch_sec   = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s -g <pgid> [-s <signal>] [-w <seconds>]\n", argv[0]);
                fprintf(stderr, "  -g  Process group ID (required)\n");
                fprintf(stderr, "  -s  Signal number to send (e.g. 15 = SIGTERM)\n");
                fprintf(stderr, "  -w  Watch group live for N seconds (1 update/sec)\n");
                return 1;
        }
    }

    if (target_pgid == -1) {
        fprintf(stderr, "Error: -g <pgid> is required.\n");
        fprintf(stderr, "Usage: %s -g <pgid> [-s <signal>] [-w <seconds>]\n", argv[0]);
        return 1;
    }
   int found = list_processes_in_group(target_pgid);

    if (signal_num != -1) {
        if (found <= 0) {
            fprintf(stderr, "\nNo processes to signal.\n");
            return 1;
        }
        printf("\nSending signal %d to process group %d...\n", signal_num, target_pgid);
        if (killpg(target_pgid, signal_num) == -1) {
            perror("killpg failed");
            return 1;
        }
        printf("Signal %d sent successfully.\n", signal_num);
    }


    if (watch_sec != -1) {
        printf("\nWatching group %d for %d second(s)...\n\n", target_pgid, watch_sec);

        SharedState *state = mmap(
            NULL,
            sizeof(SharedState),
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_SHARED,
            -1, 0
        );

        if (state == MAP_FAILED) {
            perror("mmap failed");
            return 1;
        }

        state->stop  = 0;
        state->count = 0;

        MonitorArgs args = { target_pgid, 1, state };

        pthread_t tid;
        if (pthread_create(&tid, NULL, monitor_thread, &args) != 0) {
            perror("pthread_create failed");
            munmap(state, sizeof(SharedState));
            return 1;
        }

        sleep(watch_sec);

        state->stop = 1;
        pthread_join(tid, NULL);

        printf("\nWatch complete. Last recorded count: %d process(es).\n", state->count);

        if (munmap(state, sizeof(SharedState)) == -1)
            perror("munmap failed");
    }

    return 0;
}

//Ali.Ouhabaz