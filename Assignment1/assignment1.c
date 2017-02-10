#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX 128

//Different states
enum state {READY, NOT_READY, FINISHED, RUNNING, IDLE};

//Structure of a process
typedef struct process {
    int time_arrived;
    int time_finished;
    int wait;
    char process_id[MAX];
    int burst;
    enum state curr_state;
}process;

//Defined functions
void roundRobin(process * processes, int quantum, int processcount, int runfor);
void firstComeFirstServed(process * processes, int processcount, int runfor);
char *findValue(char * line, char * name, char * sub);
int deQueue();
void insert(int data);
int size();
bool isFull();
bool isEmpty();

//Variable init.
int queue[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

const char * outputFile = "processes.out";

void main(void)
{
    char filename[20] = "input2.in";
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

        }
        else if((sub = strstr(line, "quantum")) != NULL)
        {

            //if use moude = rr then find the quantum
            quantum = atoi(findValue(line, "quantum", sub));
        }
        else if((sub = strstr(line, "process name")) != NULL)
        {
            //Breaks the while loop when it reaches the first process
            break;
        }

    }

    int i;

    //Holds each process in an array
    process processes[processcount];

    //Get each process and its required information
    for(i = 0; i <processcount; i++ )
    {

        strcpy(processes[i].process_id, findValue(line, "name", strstr(line, "name")));
        processes[i].time_arrived = atoi(findValue(line, "arrival", strstr(line, "arrival")));
        processes[i].burst = atoi(findValue(line, "burst", strstr(line, "burst")));
        processes[i].curr_state = NOT_READY;
        processes[i].time_finished = -1;
        processes[i].wait = 0;

        fgets(line, sizeof line, ifp);

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
        firstComeFirstServed(processes, processcount, runfor);
        break;
    case 's':
        printf("Test sjf\n");
        //run the sjf scheduler
        break;
    default:
        break;
    }

    printf("\nRan the %s algorithm!\n", &use_mode);
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

    ofp = fopen(outputFile, "w");

    //prints the header to the output file
    fprintf(ofp, "%d processes\n", processcount);
    fprintf(ofp, "Using Round-Robin\n");
    fprintf(ofp, "Quantum %d\n\n", quantum);

    //counts the number of active processes
    int active_procsses = 0;

    //holds the current process num
    int curr_process;

    //Stores the state of the CPU
    enum state cpu;
    cpu = IDLE;

    for(i = 0; i < runfor; i++)
    {

        //checks if there are still processes to arrive
        if((active_procsses != processcount))
        {
            int j;
            for(j = 0; j < processcount; j++)
            {
                //finds the correct process arriving at time i
                if(processes[j].time_arrived == i)
                {
                    fprintf(ofp,"Time %d: %s arrived\n", i, &processes[j].process_id);
                    processes[j].curr_state = READY;

                    //enqueueing the process
                    insert(j);
                    active_procsses++;
                }

            }

        }

        //if there is a process currently running
        if(cpu == RUNNING)
        {
            //adjust the boost
            processes[curr_process].burst -= 1;

            //calculates the wait time of the other processes
            int k;
            for(k = 0; k < processcount; k++)
            {
                //Change the (i-1) to i
                if(k != curr_process && processes[k].time_arrived < i && processes[k].curr_state != FINISHED)
                    processes[k].wait++;
            }

            //if the process has finished, output the information
            if(processes[curr_process].burst == 0)
            {
                fprintf(ofp, "Time %d: %s finished\n", i, processes[curr_process].process_id);
                processes[curr_process].time_finished = i;
                processes[curr_process].curr_state = FINISHED;
                cpu = IDLE;
            }
            //for Round-robin process can only run for a specified amount of time, this checks if that time has been reached
            else if ( i % quantum == 0)
            {
                //processes[curr_process].wait++;
                //Enqueues the current process
                insert(curr_process);

                //sets cpu to idle
                cpu = IDLE;
            }
        }

        //checks if the queue is empty and the cpu is idle
        //if so dequeue the process and run it
        if(!isEmpty() && (cpu == IDLE))
        {
            curr_process = deQueue();
            fprintf(ofp, "Time %d: %s selected (burst %d)\n", i, processes[curr_process].process_id, processes[curr_process].burst);
            processes[curr_process].curr_state = NOT_READY;
            cpu = RUNNING;
        }else if (i % quantum == 0){
            fprintf(ofp,"Time %d: Idle\n", i);
        }

    }
    fprintf(ofp, "Finished at time %d\n\n", runfor);

    for(i = 0; i < processcount; i++)
    {
        if(processes[i].time_finished != -1)
            fprintf(ofp, "%s wait %d turnaround %d\n", processes[i].process_id, processes[i].wait, (processes[i].time_finished - processes[i].time_arrived));
        else
            fprintf(ofp, "%s wait %d process did not finish\n", processes[i].process_id, processes[i].wait);

    }
    fclose(ofp);
}

void firstComeFirstServed(process * processes, int processcount, int runfor)
{
    int i;
    FILE *ofp;

    ofp = fopen(outputFile, "w");

    //prints the header to the output file
    fprintf(ofp, "%d processes\n", processcount);
    fprintf(ofp, "Using First-Come First-Served\n");

    //counts the number of active processes
    int active_procsses = 0;

    //holds the current process num
    int curr_process;

    //Stores the state of the CPU
    enum state cpu;
    cpu = IDLE;

    for(i=0; i<runfor; i++) {

        //checks if there are still processes to arrive
        if((active_procsses != processcount))
        {
            int j;
            for(j = 0; j < processcount; j++)
            {
                //finds the correct process arriving at time i
                if(processes[j].time_arrived == i)
                {
                    fprintf(ofp,"Time %d: %s arrived\n", i, &processes[j].process_id);
                    processes[j].curr_state = READY;

                    //enqueueing the process
                    insert(j);
                    active_procsses++;
                }

            }

        }

        // if cpu == RUNNING
        if(cpu == RUNNING)
        {
            //adjust the boost
            processes[curr_process].burst -= 1;

            //calculates the wait time of the other processes
            int k;
            for(k = 0; k < processcount; k++)
            {
                //Change the (i-1) to i
                if(k != curr_process && processes[k].time_arrived < i && processes[k].curr_state != FINISHED)
                    processes[k].wait++;
            }

            //if the process has finished, output the information
            if(processes[curr_process].burst == 0)
            {
                fprintf(ofp, "Time %d: %s finished\n", i, processes[curr_process].process_id);
                processes[curr_process].time_finished = i;
                processes[curr_process].curr_state = FINISHED;
                cpu = IDLE;
            }
        }

        // if !isEmpty() && spu == IDLE
        if(!isEmpty() && (cpu == IDLE))
        {
            curr_process = deQueue();
            fprintf(ofp, "Time %d: %s selected (burst %d)\n", i, processes[curr_process].process_id, processes[curr_process].burst);
            processes[curr_process].curr_state = NOT_READY;
            cpu = RUNNING;
        }
        else if (processes[curr_process].burst > 0 && cpu == RUNNING)
        {
            //fprintf(ofp, "Time %d: %s selected (burst %d)\n", i, processes[curr_process].process_id, processes[curr_process].burst);
        }
        else
        {
            fprintf(ofp,"Time %d: Idle\n", i);
        }
    }

    fprintf(ofp, "Finished at time %d\n\n", runfor);

    for(i = 0; i < processcount; i++)
    {
        if(processes[i].time_finished != -1)
            fprintf(ofp, "%s wait %d turnaround %d\n", processes[i].process_id, processes[i].wait, (processes[i].time_finished - processes[i].time_arrived));
        else
            fprintf(ofp, "%s wait %d process did not finish\n", processes[i].process_id, processes[i].wait);

    }
    fclose(ofp);
}


//Queue functions below
//-----------------------//

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}

void insert(int data) {

   if(!isFull()) {

      if(rear == MAX-1) {
         rear = -1;
      }

      queue[++rear] = data;
      itemCount++;
   }
}

int deQueue() {
   int data = queue[front++];

   if(front == MAX) {
      front = 0;
   }

   itemCount--;
   return data;
}
