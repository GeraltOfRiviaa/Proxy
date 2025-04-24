#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LOGFILE "datalog.csv"
#define OUTPUTFILE "output.txt"


typedef struct {
    int hour;
    int min;
} TIME;

typedef struct {
    char id[10];
    TIME entry;
    TIME end;
    int bytes;
} LOG;

LOG *readLogs(LOG *logs, int *lines) {
    FILE *fr = fopen(LOGFILE, "r");
    if (fr == NULL) {
        printf("error opening path: %s", LOGFILE);
        return NULL;
    }
    char line[100];
    int i = 0;
    int j = 0;
    LOG *temp = NULL;
    while (fgets(line, sizeof(line), fr) != NULL) {
        i = i + 1; // i+=1 , i++
        temp = (LOG *) realloc(logs, i * sizeof(LOG));
        if (temp == NULL) {
            printf("Allocation failed.");
            free(logs);
            return NULL;
        }
        logs = temp;
        char *pch = strtok(line, " :");

        j = 0;
        while (pch != NULL) {
            switch (j) {
                case 0:
                    strcpy(logs[i - 1].id, pch);
                    break;
                case 1:
                    logs[i - 1].entry.hour = atoi(pch);
                    break;
                case 2:
                    logs[i - 1].entry.min = atoi(pch);
                    break;
                case 3:
                    logs[i - 1].end.hour = atoi(pch);
                    break;
                case 4:
                    logs[i - 1].end.min = atoi(pch);
                    break;
                case 5:
                    logs[i - 1].bytes = atoi(pch);
                    break;
                default:
                    printf("error in switch statement - retard");
                    break;
            }
            pch = strtok(NULL, " :");

            j = j + 1;
        }
    }
    *lines = i - 1;
    fclose(fr);
    return logs;
}

int totalMins(TIME time) {
    return time.hour*60+time.min;
}


void swap(LOG* xp, LOG* yp){
    LOG temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(LOG *logs, int lines){
    int i, j;
    int swapped;
    for (i = 0; i < lines - 1; i++) {
        swapped = 0;
        for (j = 0; j < lines - i - 1; j++) {
            if (atoi(logs[j].id+2) > atoi(logs[j + 1].id+2)) {
                swap(&logs[j], &logs[j + 1]);
                swapped = 1;
            }
        }
        if (swapped == 0)
            break;
    }
}

void print(LOG *logs, int lines) {
    for (int i = 0; i < lines; i++) {
        int difference=totalMins(logs[i].end)-totalMins(logs[i].entry);
        printf("%s %2d:%02d %2d:%02d %2d %9d\n", logs[i].id, logs[i].entry.hour, logs[i].entry.min,
               logs[i].end.hour, logs[i].end.min, difference, logs[i].bytes);
    }
}

void write(LOG *logs, int lines) {
    FILE *fw = fopen(OUTPUTFILE, "a");
    int current = 0;
    int totaldiff = 0;
    int totalbytes = 0;
    int difference = 0;

    fprintf(fw, " pc  start  konec  minuty   data  \n");
    for (int i=0; i<lines; i++) {
        if (current != atoi(logs[i].id+2)) {
            fprintf(fw, "PC%d        %d min    %d b\n", current, totaldiff, totalbytes);
            current = atoi(logs[i].id+2);
            totaldiff=totalMins(logs[i].end)-totalMins(logs[i].entry);
            totalbytes=logs[i].bytes;

            fprintf(fw, "    %2d:%02d %2d:%02d %2d %9d\n", logs[i].entry.hour, logs[i].entry.min,
               logs[i].end.hour, logs[i].end.min, difference, logs[i].bytes);
        }else {
            totaldiff+=(totalMins(logs[i].end)-totalMins(logs[i].entry));
            totalbytes+=logs[i].bytes;
            difference=totalMins(logs[i].end)-totalMins(logs[i].entry);
            fprintf(fw, "    %2d:%02d %2d:%02d %2d %9d\n", logs[i].entry.hour, logs[i].entry.min,
               logs[i].end.hour, logs[i].end.min, difference, logs[i].bytes);
        }
    }
    fprintf(fw, "PC%d        %d min    %d b\n", current, totaldiff, totalbytes);

}

int main(void) {
    LOG *logs = NULL;
    int lines = 0;

    logs = readLogs(logs, &lines);
    bubbleSort(logs, lines);
    print(logs, lines);
    write(logs, lines);
    free(logs);
    return 0;
}
