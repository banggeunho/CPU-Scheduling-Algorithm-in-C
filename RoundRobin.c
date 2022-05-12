#include <stdio.h>
#include <stdlib.h>

#define DEFAULT 0
#define RUNNABLE 1
#define RUNNING 2
#define TERMINATED 3

struct node {
    int p, priority, state, cpuDoneTime, arriveTime, burstTime, completeTime, waitTime, taTime, responseTime, flag, remainTime; // pid, priority, arrive, burst, complete, waiting, turn-around
    struct node* next;
};//Structure declaration

struct node *start = NULL;

int isTerminated(struct node* start)
{
    struct node *ptr;
    int total = 0;

    ptr = start;
    while (ptr != NULL) {
        if (ptr->state != TERMINATED)
            total++;
        // printf("  %d ",ptr->state);
        ptr = ptr->next;
    }
    // printf("\n");
    return total;
}
int RR(struct node* start, int currentTime, int time_quantum)
{
    
    struct node* ptr, *temp_ptr;

    // 프로세스 도착
    ptr = start;
    while (ptr != NULL) {
        
        if (ptr -> state == DEFAULT)
        {
            if (ptr->arriveTime == currentTime)
            {
                ptr->state = RUNNABLE;
                printf("\n<time %d> [new arrival] process %d / done : %d / burst: %d", currentTime, ptr->p, ptr->cpuDoneTime, ptr->burstTime);
            }
        }
        ptr = ptr->next;
    }


    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNING) // 돌아가는 애가 끝날떄;;
        {
            if (ptr->cpuDoneTime == ptr->burstTime)
            {
                ptr->state = TERMINATED;
                ptr->completeTime = currentTime;
                ptr->taTime = ptr->completeTime - ptr->arriveTime;
                ptr->waitTime = ptr->taTime - ptr->burstTime;
                break;
            }

            if (ptr->remainTime > 0)
            {
                ptr->remainTime--;
                ptr->cpuDoneTime++;
                return ptr->p;
            }
            else
            {
                ptr->state = RUNNABLE;
                ptr->flag = 1; //얘 이번타임은 다 돌렸음..
                ptr->remainTime = time_quantum;
                // printf("\n%d 다음 기회로  %d  %d \n", ptr->p, ptr->state, ptr->remainTime);
                break;
            }
        }
        ptr = ptr->next;
    }

    int duration = 100000000;
    int earlyArrival = -1;
    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNABLE && ptr->flag == 0)
        {
            if (ptr->arriveTime < duration)
            {
                duration = ptr->arriveTime;
                earlyArrival = ptr -> p;
            }
        }
        ptr = ptr->next;
    }

    // 아직 안돌린 애가 있을 경우
    ptr = start;
    while (ptr != NULL) {
        if(ptr->p == earlyArrival && ptr->state==RUNNABLE && ptr->flag == 0)
        {
            ptr->state = RUNNING;
            ptr->cpuDoneTime++;
            ptr->remainTime--;
            return ptr->p;
        }
        ptr = ptr->next;
    }

    duration = 100000000;
    earlyArrival = -1;

    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNABLE && ptr->flag == 1)
        {
            if (ptr->arriveTime < duration)
            {
                duration = ptr->arriveTime;
                earlyArrival = ptr -> p;
            }
        }
        ptr = ptr->next;
    }

    ptr = start;
    while (ptr != NULL) {
        if(ptr->flag == 1 && ptr->state == RUNNABLE)
        {
            ptr->flag = 0;
        }
        ptr = ptr->next;
    }

    // 다 한바퀴씩 돌았을 경우
    ptr = start;
    while (ptr != NULL) {
        if(ptr->p == earlyArrival)
        {
            ptr->state = RUNNING;
            ptr->cpuDoneTime++;
            ptr->remainTime--;
            return earlyArrival;
        }
        ptr = ptr->next;
    }

    return -1; // cpu is idle
}


int main() {
    // mode:0 --> FCFS
    // mode:1 --> RR / PSJF
    // mode:2 --> Preemptive Prioirity

    int i, n, j, temp, c, time_quantum;
    int numOfProcess = 0;
    struct node* ptr, * newnode, * tmpptr;//Declaring pointers
    float avr_wt, avr_tat, avr_rt = 0.0;
    FILE *file = fopen("input.txt", "r");

    if (file == 0)
    {
        printf( "Could not open file.\n");
    }
    else
    {   
        printf("Enter the time quamtum: ");
        scanf("%d", &time_quantum);

        // 인풋 받아와서( pid, prioriy, arrive time, busrt time) node 생성
        while (1)
        {   
            newnode = (struct node*)malloc(sizeof(struct node));/*Dynamic memory allocation to store structure data*/
            if (fscanf(file, "%d %d %d %d", &newnode->p, &newnode->priority, &newnode->arriveTime, &newnode->burstTime) != 4)
                break;
            
            newnode->cpuDoneTime = 0;
            newnode->completeTime = 0;
            newnode->waitTime = 0;
            newnode->taTime = 0;
            newnode->responseTime = 0;
            newnode->state = DEFAULT;
            newnode->flag = 0;
            newnode->remainTime = time_quantum;
            newnode->next = NULL;
   
            if (start == NULL) {
                start = newnode;
                ptr = newnode;
                }               

            else {
                ptr->next = newnode;
                ptr = ptr->next;
            }
            numOfProcess ++;
        }
        fclose(file);


            
        ptr = start;
        printf("PID\t\tPriority\tArrive\t\tBurst\t\tState\n");
        while (ptr != NULL) {
            printf("P[%d]\t\t%d\t\t%d\t\t%d\t\t%d\n ", ptr->p, ptr->priority, ptr->arriveTime, ptr->burstTime, ptr->state);
            ptr = ptr->next;
        }//displaying the contents of structure in order-wise

        int currentProcess, time, usage = 0;
        int previousPid = -1;
        printf("============================================\n");
        printf("Scheduling : Round Robin (Time Quantum: %d)\n", time_quantum);
        printf("============================================\n");

        while(isTerminated(start)) //isTerminated(start) > 0
        {   
            // printf(" %d",isTerminated(start));
            currentProcess = RR(start, time, time_quantum);
            // printf("%d\n",currentProcess);
            if (currentProcess >= 0)
            {
                usage++;
                if (time == 0)
                    previousPid = currentProcess;


                if(previousPid != currentProcess && previousPid != -1)
                {
                    printf("\n---------------(Context Switch)---------------");
                    tmpptr = start;
                    while(tmpptr != NULL)
                    {
                        if (tmpptr->p == previousPid)
                        {
                           if (tmpptr->responseTime == 0)
                           {
                               tmpptr->responseTime = (time) - tmpptr->arriveTime;
                           }
                           break;
                        }
                        tmpptr = tmpptr->next;
                    }
                }
                tmpptr = start;
                while(tmpptr != NULL)
                {
                    if (tmpptr->p == currentProcess)
                    {
                        if (tmpptr->burstTime == tmpptr->cpuDoneTime)
                        {
                            printf("\n<time %d> process %d is finished / done : %d / burst : %d / remain: %d", time, tmpptr->p, tmpptr->cpuDoneTime, tmpptr->burstTime, tmpptr->remainTime);
                        }
                        else
                            printf("\n<time %d> process %d is running / done : %d / burst : %d / remain: %d", time, tmpptr->p, tmpptr->cpuDoneTime, tmpptr->burstTime, tmpptr->remainTime);
                        break;
                    }
                    tmpptr = tmpptr->next;
                }
            }
            else
            {
                if (isTerminated(start) > 0)
                    printf("\n<time %d> ---- system is idle ----", time);
                else
                    break;
            }

            time ++;
            previousPid = currentProcess;
        }
        printf("\n<time %d> all process finish\n", time-1);
        printf("============================================\n");
        // 전체 결과 구하기
        ptr = start;
        while (ptr != NULL) {
            avr_wt += ptr->waitTime;
            avr_tat += ptr->taTime;
            if (ptr->responseTime == 0)
            {
                ptr->responseTime = ptr->taTime;
            }
            avr_rt += ptr->responseTime;
            printf("pid : %d, wait : %d, tat : %d rt : %d \n", ptr->p, ptr->waitTime, ptr->taTime, ptr->responseTime);
            ptr = ptr->next;
        }


        printf("============================================\n");
        printf("Average cpu usage : %.2f%%\n", ((double)usage/(double)(time-1)*100));
        printf("Average waiting time : %.2f\n", ((double)avr_wt/(double)numOfProcess));
        printf("Average response time : %.2f\n", ((double)avr_rt/(double)numOfProcess));
        printf("Average turnaround time : %.2f\n", ((double)avr_tat/(double)numOfProcess));

        return 0;
    }
}