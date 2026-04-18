//
//  main.c
//  305 GPC
//
//  Created by 林庭威 on 4/17/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#define OFFSET 100

void compFile(char *stringA, size_t sizeA, char *stringB, size_t sizeB) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // numA and numB: The lengths (number of characters) of our strings


    int V[200];
    for(int i = 0; i < 200; i++) V[i] = 0;

    V[OFFSET + 1] = 0;

    for (int dist = 0; dist <= sizeA + sizeB; dist++) {

        // k: The diagonal lane
        for (int k = -dist; k <= dist; k += 2) {
            int x, y;

            // Move Right (Delete) or Down (Insert)
            if (k == -dist || // if leftmost
                (k != dist && V[OFFSET + k - 1] < V[OFFSET + k + 1])) {// not right most and above is further than left
                x = V[OFFSET + k + 1]; // Move Down
            } else {
                x = V[OFFSET + k - 1] + 1; // Move Right
            }

            y = x - k; // Diagonal path

            // If chars match, just keep going no need to check
            while (x < sizeA && y < sizeB && stringA[x] == stringB[y]) {
                x++;
                y++;
            }

            V[OFFSET + k] = x; // furthest x reached for this lane

            // both reached end
            if (x >= sizeA && y >= sizeB) {
                clock_gettime(CLOCK_MONOTONIC, &end);
                double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                printf("\n--- Performance Metrics ---\n");
                printf("Total Edit Distance: %d\n", dist);
                printf("Execution Time: %.6f seconds\n", time_taken);
                return;
            }
        }
    }
}
int main(int argc, char *argv[]) {
    int opt;
    char *fileA = "C:/Users/dakot/Documents/GitHub/CSC332Midterm/filediff1.txt"; // Change the paths to test if you want
    char *fileB = "C:/Users/dakot/Documents/GitHub/CSC332Midterm/filediff2.txt";

    // update the parameters of the string "f:k:s:" to ONLY include the letters assigned to their specific feature.
    while ((opt = getopt(argc, argv, "f:k:t:c:a:b:g:s:")) != -1) {
        switch (opt) {
            case 'a': fileA = optarg; break;
            case 'b': fileB = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -a file1 -b file2\n", argv[0]);
                return 1;
        }
    }

    printf("Feature logic executing now...\n");

    if (fileA == NULL || fileB == NULL) {
        printf("Error: You must provide both -a and -b strings to compare.\n");
        printf("Usage: ./program -a hello -b cello\n");
        return 1;
    }

    // 3. EXECUTE the algorithm with the data we collected
        int fdA = open(fileA, O_RDONLY);
        struct stat stA;
        fstat(fdA, &stA);
        char *mapA = mmap(NULL, stA.st_size, PROT_READ, MAP_PRIVATE, fdA, 0);

        int fdB = open(fileB, O_RDONLY);
        struct stat stB;
        fstat(fdB, &stB);
        char *mapB = mmap(NULL, stB.st_size, PROT_READ, MAP_PRIVATE, fdB, 0);

        if (mapA == MAP_FAILED || mapB == MAP_FAILED) {
            perror("mmap failed");
            return 1;
        }

        printf("Comparing %s and %s using memory mapping...\n", fileA, fileB);
        compFile(mapA, stA.st_size, mapB, stB.st_size);
    return 0;
}

