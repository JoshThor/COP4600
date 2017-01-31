#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX 100

enum state {READY, NOT_READY, RUNNING, FINISHED};

typedef struct process {
    int time_arrived;
    char process_id[256];
    int burst;
    enum state curr_state;
}process;


void roundRobin(process * processes, int quantum, int processcount, int runfor);
char *findValue(char * line, char * name, char * sub);
process deQueue();
void insert();
int size();
bool isFull();
bool isEmpty();
process peek();



process queue[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;


void main(void)
{
    char filename[20] = "input.in";
    char use_mode[5];
    char line[256];

    int processcount = 0, runfor = 0, quantum = 0;

    FILE *ifp;

    ifp = fopen(filename, "r");
    if(ifp == NULL)
    {
        printf("Error finding file.\n");
        exit(1);
    }

    while(fgets(line, sizeof line, ifp) != NULL)
    {
        //Prints input
        fputs(line, stdout);


        //first remove comment in the line
        char *comment = strchr(line, '#');
        if(comment)
        {
            int index = comment - line;
            line[index] = '\0';
        }

        //finds the processcount in the first line
        char *sub;
        if((sub = strstr(line, "processcount")) != NULL)
        {
            processcount = atoi(findValue(line, "processcount", sub));
        }
        else if((sub = strstr(line, "runfor")) != NULL)
        {
            //Finds runfor
            runfor = atoi(findValue(line, "runfor", sub));
        }
        else if((sub = strstr(line, "use")) != NULL)
        {
            //Finds the use mode
            strcpy(use_mode, findValue(line, "use", sub));

            if(strcmp(use_mode, "rr") != 0)
                break;

        }
        else if((sub = strstr(line, "quantum")) != NULL)
        {

            //if use moude = rr then this block find the quantum
            quantum = atoi(findValue(line, "quantum", sub));
            break;
        }

    }

    int i;

    //Holds each process in an array
    process processes[processcount];

    //Get each process and its required information
    for(i = 0; i <processcount; i++ )
    {
        fgets(line, sizeof line, ifp);

        strcpy(processes[i].process_id, findValue(line, "name", strstr(line, "name")));
        processes[i].time_arrived = atoi(findValue(line, "arrival", strstr(line, "arrival")));
        processes[i].burst = atoi(findValue(line, "burst", strstr(line, "burst")));
        processes[i].curr_state = NOT_READY;

    }
    //close input file
    fclose(ifp);

    //call the appropriate function here
    switch(use_mode[0])
    {
    case 'r':
        //run the rr scheduler
        roundRobin(processes, quantum, processcount, runfor);
        break;
    case 'f':
        //run the fcfs scheduler
        break;
    case 's':
        //run the sjf scheduler
        break;
    default:
        break;
    }

    printf("\nRan processes successfully!\n");
}

//Finds the value associated with the given word
char * findValue(char *line, char *name, char *sub)
{
    int value;
    int pos = (sub - line)+ strlen(name);

    int i = 0;
    while(line[pos + i] == ' ')
    {
        i++;
    }

    static char num[256] = "";
    int j = 0;
    while(line[pos + i] != ' ' && line[pos + i] != '\0' && line[pos + i] != '\n')
    {
        num[j++] = line[pos+i];
        i++;
    }
    num[j] = '\0';

    return num;
}

//Round Robin Scheduler algorithm
void roundRobin(process * processes, int quantum, int processcount, int runfor)
{
    int i;
    FILE *ofp;

    ofp = fopen("processes.out", "w");

    fprintf(ofp, "%d processes\n", processcount);
    fprintf(ofp, "Using Round-Robin\n");
    fprintf(ofp, "Quantum %d\n\n", quantum);

    int active_procsses = 0;
    process curr;
    curr.curr_state = READY;
    for(i = 0; i < runfor; i++)
    {
        if((active_procsses != processcount))
        {
            int j;
            for(j = 0; j < processcount; j++)
            {
                if(processes[j].time_arrived == i)
                {
                    fprintf(ofp,"Time %d: %s arrived\n", i, &processes[j].process_id);
                    processes[j].curr_state = READY;
                    insert(processes[j]);
                    active_procsses++;
                }

            }

        }

        if(curr.curr_state == RUNNING)
        {
            curr.burst -= 1;

            if(curr.burst == 0)
            {
                fprintf(ofp, "Time %d: %s finished\n", i, curr.process_id);
                curr.curr_state = FINISHED;
            }
            else if ( i % quantum == 0)
            {
                insert(curr);
                curr.curr_state = READY;
            }
        }

        if(!isEmpty() && (curr.curr_state == READY || curr.curr_state == FINISHED))
        {
            curr = deQueue();
            fprintf(ofp, "Time %d: %s selected (burst %d)\n", i, curr.process_id, curr.burst);
            curr.curr_state = RUNNING;
        }else if (i % quantum == 0){
            fprintf(ofp,"Time %d: Idle\n", i);
            continue;
        }

    }
    fprintf(ofp, "Finished at time %d\n", runfor);
    fclose(ofp);
}


//Queue functions below
//-----------------------//
process peek() {
   return queue[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}

void insert(process data) {

   if(!isFull()) {

      if(rear == MAX-1) {
         rear = -1;
      }

      queue[++rear] = data;
      itemCount++;
   }
}

process deQueue() {
   process data = queue[front++];

   if(front == MAX) {
      front = 0;
   }

   itemCount--;
   return data;
}
