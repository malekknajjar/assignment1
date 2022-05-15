#include "List.h"
#include "string.h"
#define maxStringLength 1000

struct PCB
{
    char processName;
    int arrivalTime;
    int burstTime;
    int priority;
    int processedTime;
    int responseTime;
    int completionTime;
};

struct Result
{
    double avgTAT;
    double avgRT;
    char* Timeline;
};

struct Result* roundRobin(int timeSlice, struct List* list) {
    char* Timeline = (char*)malloc(sizeof(char) * maxStringLength);
    int currentTime = 0;
    
    struct Node* iterator = list->head;
    
    struct List Plist;
    ListInit(&Plist);

    while (!ListIsEmpty(list)) {
        int localprocessedTime = 0;
        struct PCB* pcb = (struct PCB*)iterator->data;

        if (pcb->processedTime == 0) {
            pcb->responseTime = currentTime - pcb->arrivalTime;
        }

        if (timeSlice + pcb->processedTime < pcb->burstTime) {
            pcb->processedTime += timeSlice;
            localprocessedTime = timeSlice;
        }
        else { 
            int temp = pcb->burstTime - pcb->processedTime;
            pcb->processedTime += temp;
            localprocessedTime = temp;
        }
        
        currentTime += localprocessedTime;

        char temp[maxStringLength];
        snprintf(temp, maxStringLength, "%c(%d-->%d), ",
            pcb->processName, currentTime - localprocessedTime, currentTime);

        strcat(Timeline, temp);

        if (pcb->processedTime == pcb->burstTime) {
            pcb->completionTime = currentTime;    
            AddNodeinList(&Plist, DeleteNodefromList(list, pcb));
        }
        iterator = iterator->next;
    }

    double TAT = 0, RT = 0;
    struct Node* it = Plist.head;
    int i = 1;
    int count = 0;
    do {
        struct PCB* pcb = (struct PCB*)it->data;

        
        TAT += pcb->completionTime - pcb->arrivalTime;
        RT += pcb->responseTime;
        count++;

        it = it->next;
    } while (it != Plist.head);

    clearList(&Plist);

    Timeline[strlen(Timeline) - 2] = '\0';
    
    struct Result* result = (struct Result*)malloc(sizeof(struct Result));
    result->avgRT = RT/count;
    result->avgTAT = TAT/count;
    result->Timeline = Timeline;
    
    return result;
}

struct Node* getHighestPriority(int currentTime, struct List* list) {
    struct Node* curr = list->head;
    struct Node* result = curr;
    int min = ((struct PCB*)result->data)->priority;

    while (curr != list->tail) {
        curr = curr->next;
        struct PCB* pcb = (struct PCB*)curr->data;
        if (currentTime >= pcb->arrivalTime && min > pcb->priority) {
            result = curr;
            min = pcb->priority;
        }
    }
    struct PCB* pcb = (struct PCB*)curr->data;
    if (currentTime >= pcb->arrivalTime && min > pcb->priority) {
        result = curr;
        min = pcb->priority;
    }
    
    return result;
}

struct Result* PriorityScheduling(struct List* list) {
    char* Timeline = (char*)malloc(sizeof(char) * maxStringLength);
    int currentTime = 0;
        
    struct List Plist;
    ListInit(&Plist);

    while (!ListIsEmpty(list)) {
        struct PCB* pcb = (struct PCB*)getHighestPriority(currentTime, list)->data;
        pcb->responseTime = currentTime - pcb->arrivalTime;
        pcb->processedTime = pcb->burstTime;
        currentTime += pcb->processedTime;
        pcb->completionTime = currentTime;
        
        char temp[maxStringLength];
        snprintf(temp, maxStringLength, "%c(%d-->%d), ",
            pcb->processName, currentTime - pcb->burstTime, currentTime);

        strcat(Timeline, temp);
        
        AddNodeinList(&Plist, DeleteNodefromList(list, pcb));
    }

    double TAT = 0, RT = 0;
    struct Node* it = Plist.head;
    int i = 1;
    int count = 0;
    do {
        struct PCB* pcb = (struct PCB*)it->data;
        TAT += pcb->completionTime - pcb->arrivalTime;
        RT += pcb->responseTime;
        count++;

        it = it->next;
    } while (it != Plist.head);

    clearList(&Plist);

    Timeline[strlen(Timeline) - 2] = '\0';
    
    struct Result* result = (struct Result*)malloc(sizeof(struct Result));
    result->avgRT = RT/count;
    result->avgTAT = TAT/count;
    result->Timeline = Timeline;
    
    return result;
}

int stringToNumber(char *str)
{
    int num = 0;

    while (*str != '\0') {
        if ((*str >= '0') && (*str <= '9')) {
            num = (num * 10) + (*str - '0');//subtracting ascii of 0
        }
        else {
            return 0;
        }
        str++;
    }
    return num;
}

int main(int argc, char*argv[]) {
    // gcc program.c -o program
    // ./program algo timeSlice fileName
    // ./program RR 20 input.txt
    // ./program PR 00 input.txt
    
    if (argc < 3) {
        printf("Wrong No. parameters passed.\n");
        return 0;
    }

    const char* schedulingAlg = argv[1];
    int timeSlice = stringToNumber(argv[2]);
    const char* fileName = argv[3];

    printf("timeSlice: %d\n", timeSlice);
    printf("scheduledAlg: %s\n", schedulingAlg);
    printf("fileName: %s\n", fileName);

    int choice = 0;

    if (strcmp(schedulingAlg, "RR") == 0) {
        choice = 1;
        if (timeSlice == 0) {
            printf("Invalid input! %s is not a suitable input for timeSlice.\n", argv[2]);
            return 0;
        }
    }
    else if (strcmp(schedulingAlg, "PR") == 0) {
        choice = 2;
    }
    else {
        printf("Invalid parameters passed.\n");
        return 0;
    }

    FILE* fp;
    fp = fopen(fileName, "r");
    if (!fp) {
        printf("File \"%s\" not found.\n", fileName);
        return 0;
    }

    struct List list;
    ListInit(&list);

    char currentChar = 'A';
    int currentTime = 0;
    char buffer[50];
    while (fscanf(fp, "%s", buffer) != EOF) {
        if (strcmp("proc", buffer) == 0) {
            struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));

            pcb->processName = currentChar++;
            pcb->arrivalTime = currentTime;
            pcb->processedTime = 0;
            pcb->responseTime = 0;
            pcb->completionTime = 0;
            fscanf(fp, "%d", &pcb->priority);
            fscanf(fp, "%d", &pcb->burstTime);
            
            AddNodeinList(&list,pcb);
        }
        else if (strcmp("idle", buffer) == 0) {
            fscanf(fp, "%d", &currentTime);
        }
    }

    fclose(fp);

    struct Result* result = choice == 1 ? roundRobin(timeSlice, &list) : PriorityScheduling(&list);

    printf("Input File Name: %s\n\n", fileName);
    choice == 1 ? printf("CPU Scheduling Alg: %s (%d)\n\n", schedulingAlg, timeSlice) : 
        printf("CPU Scheduling Alg: %s\n\n", schedulingAlg);
    printf("Avg. Turnaround Time: %0.2lf ms\n\n", result->avgTAT);
    printf("Avg. Response Time in R Queue: %0.2lf ms\n\n", result->avgRT);
    printf("CPU Timeline: %s\n", result->Timeline);

    if (result) {
        if (result->Timeline)
            free(result->Timeline);
        free(result);
    }
}