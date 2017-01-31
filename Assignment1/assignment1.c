#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum state {READY, NOT_READY};

typedef struct process {
    int time_arrived;
    char process_id[256];
    int burst;
    enum state curr_state;
}process;

char *findvalue(char * line, char * name, char * sub);

void main(void)
{
    char filename[20] = "input.txt";
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

            int pos = (sub - line)+ strlen("processcount");

            int i = 0;
            while(line[pos + i] == ' ')
            {
                i++;
            }

            char num[256];
            int j = 0;
            while(line[pos + i] != ' ' && line[pos + i] != '\0' && line[pos + i] != '\n')
            {
                num[j] = line[pos+i];
                //printf("%s\n", &num);
                i++;
                j++;
            }
            processcount = atoi(num);

        }
        else if((sub = strstr(line, "runfor")) != NULL)
        {
            runfor = atoi(findvalue(line, "runfor", sub));
        }
        else if((sub = strstr(line, "use")) != NULL)
        {
            strcpy(use_mode, findvalue(line, "use", sub));

            if(strcmp(use_mode, "rr") != 0)
                break;

        }
        else if((sub = strstr(line, "quantum")) != NULL)
        {
            quantum = atoi(findvalue(line, "quantum", sub));
            break;
        }

    }

    int i;

    process processes[processcount];

    for(i = 0; i <processcount; i++ )
    {
        fgets(line, sizeof line, ifp);

        strcpy(processes[i].process_id, findvalue(line, "name", strstr(line, "name")));
        processes[i].time_arrived = atoi(findvalue(line, "arrival", strstr(line, "arrival")));
        processes[i].burst = atoi(findvalue(line, "burst", strstr(line, "burst")));
        processes[i].curr_state = NOT_READY;

    }

    for(i = 0; i <processcount; i++)
    {
        printf("Process: %s\n",&processes[i].process_id);
        printf("Arival: %d\n",processes[i].time_arrived);
        printf("Burst: %d\n",processes[i].burst);
        printf("Current state: %d\n", processes[i].curr_state);
    }

    //call the appropriate function here
        //run the rr scheduler
        //run the fcfs scheduler
        //run the sjf scheduler

    fclose(ifp);

}

char * findvalue(char *line, char *name, char *sub)
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

