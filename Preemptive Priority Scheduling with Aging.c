#include <stdio.h>
#include <stdlib.h>

#define DEFAULT 0
#define RUNNABLE 1
#define RUNNING 2
#define TERMINATED 3

struct node {
    int p, state, cpuDoneTime, arriveTime, burstTime, completeTime, waitTime, taTime, responseTime; // pid, priority, arrive, burst, complete, waiting, turn-around
    float priority, new_priority;
    struct node* next;
};//Structure declaration

struct node *start = NULL;

int isTerminated(struct node* start)
{
    struct node *ptr;
    int total = 0;
    int i = 0;
    ptr = start;
    while (ptr != NULL) {
        if (ptr->state != TERMINATED)
            total++;
        // printf("   %d   ", ptr->state);
        ptr = ptr->next;
    }
    // printf("\n");
    return total;
}

float Aging(float alpha, int t, int priority)
{
    return priority + alpha*t;
}

int PPS(struct node* start, int currentTime)
{
    
    struct node* ptr, *temp_ptr;
    ptr = start;

    // 프로세스가 큐에 도착시 사용가능한 상태로 변경
    while (ptr != NULL) {
        
        if (ptr -> state == DEFAULT)
        {
            if (ptr->arriveTime == currentTime)
            {
                ptr->state = RUNNABLE;
                printf("\n<time %d> [new arrival] process %d / done : %d / burst: %d / priority : %.1f", currentTime, ptr->p, ptr->cpuDoneTime, ptr->burstTime, ptr->priority);
            }
        }
        ptr = ptr->next;
    }


    // Aging 
    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNABLE && ptr->arriveTime < currentTime)
        {
            ptr->waitTime ++;
            printf("   P[%d]   %.1f ----> ", ptr-> p, ptr->new_priority);
            ptr->new_priority = Aging(0.5, ptr->waitTime, ptr->priority);
            printf(" %.1f", ptr->new_priority);
        }
        ptr = ptr->next;
    }

    float tmp_priority = 0;
    int highest = 0;
    int highest_cpuDoneTime = 0;
    int highest_burstTime = 0;
    int highest_state = 0;
    int highest_priority = 0;



    // 최고로 priority가 높은 값을 찾아난다.
    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNABLE)
        {
            tmp_priority = ptr->new_priority;
            break;
        }
        ptr = ptr->next;
    }
    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNABLE)
        {
            if (ptr->new_priority >= tmp_priority)
            {
                tmp_priority = ptr->new_priority;
                highest = ptr -> p;
                highest_priority = ptr->new_priority;
                highest_cpuDoneTime = ptr -> cpuDoneTime; // 가장 큰새1끼 추출
                highest_burstTime = ptr -> burstTime;
                highest_state = ptr -> state;
            }
        }
        ptr = ptr->next;
    }
    // printf("highest: %d highest_priority : %d\n", highest, highest_priority);
    // preempt 하는 부분
    ptr = start;
    while (ptr != NULL) {
        if (ptr -> state == RUNNING)
        {
            if (ptr->cpuDoneTime == ptr->burstTime)
            {
                ptr->state = TERMINATED;
                ptr->completeTime = currentTime;
                ptr->taTime = ptr->completeTime - ptr->arriveTime;
                ptr->waitTime = ptr->taTime - ptr->burstTime;
                break;
            }
            else if (highest_priority > ptr->new_priority && highest_state == RUNNABLE)
            {
                ptr->state = RUNNABLE;
                // 최소인 새1끼 찾아서 새로운 값으로 저장
                temp_ptr = start;
                while (temp_ptr != NULL) {
                    if (temp_ptr -> p == highest)
                    {
                        temp_ptr -> state = RUNNING;
                        temp_ptr -> cpuDoneTime ++;
                        return temp_ptr -> p;
                    }
                    temp_ptr = temp_ptr->next;
                }
            }
            else{
                ptr->cpuDoneTime++;
                return ptr->p;
            }
        }
        ptr = ptr->next;
    }

    if (highest_state == RUNNABLE)
    {
        temp_ptr = start;
        while (temp_ptr != NULL) {
            if (temp_ptr -> p == highest && temp_ptr->cpuDoneTime < temp_ptr->burstTime)
            {
                temp_ptr -> state = RUNNING;
                temp_ptr -> cpuDoneTime ++;
                return temp_ptr->p;
            }
            temp_ptr = temp_ptr->next;
        }
    }
    return -1; // cpu is idle
}

int main() {

    int i, n, j, temp, c;
    int numOfProcess = 0;
    struct node* ptr, * newnode, * tmpptr;//Declaring pointers
    float avr_wt = 0.0, avr_tat, avr_rt = 0.0;
    FILE *file = fopen("input.txt", "r");

    if (file == 0)
    {
        printf( "Could not open file.\n");
    }
    else
    {   
        // 인풋 받아와서( pid, prioriy, arrive time, busrt time) node 생성
        while (1)
        {   
            newnode = (struct node*)malloc(sizeof(struct node));/*Dynamic memory allocation to store structure data*/
            if (fscanf(file, "%d %f %d %d", &newnode->p, &newnode->priority, &newnode->arriveTime, &newnode->burstTime) != 4)
                break;
            
            newnode->new_priority = newnode->priority;
            newnode->cpuDoneTime = 0;
            newnode->completeTime = 0;
            newnode->waitTime = 0;
            newnode->taTime = 0;
            newnode->responseTime = 0;
            newnode->state = DEFAULT;
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
            printf("P[%d]\t\t%.1f\t\t%d\t\t%d\t\t%d\n ", ptr->p, ptr->priority, ptr->arriveTime, ptr->burstTime, ptr->state);
            ptr = ptr->next;
        }//displaying the contents of structure in order-wise

        int currentProcess, time, usage = 0;
        int previousPid = -1;
        printf("============================================\n");
        printf("Scheduling : Preemtive Prioirty Sceduling\n");
        printf("============================================\n");

        while(isTerminated(start) > 0) // isTerminated(start) > 0
        {   
            currentProcess = PPS(start, time);
            // printf("%d\n",currentProcess);
            if (currentProcess >= 0)
            {
                usage++;

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
                            printf("\n<time %d> process %d is finished / done : %d / burst : %d / priority : %.1f", time, tmpptr->p, tmpptr->cpuDoneTime, tmpptr->burstTime, tmpptr->new_priority);
                        }
                        else
                            printf("\n<time %d> process %d is running / done : %d / burst : %d / priority : %.1f", time, tmpptr->p, tmpptr->cpuDoneTime, tmpptr->burstTime, tmpptr->new_priority);
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
            printf("pid : %d, wait : %d, tat : %d rt : %d,  base priority: %.1f, new priority: %.1f\n", ptr->p, ptr->waitTime, ptr->taTime, ptr->responseTime, ptr->priority, ptr->new_priority);
            ptr = ptr->next;
        }


        printf("============================================\n");
        printf("Average cpu usage : %.2f%%\n", ((double)usage/(double)(time-1)*100));
        printf("Average waiting time : %.2f\n", ((double)avr_wt/(double)numOfProcess));
        printf("Average response time : %.2f\n", ((double)avr_rt/(double)numOfProcess));
        printf("Average turnaround time : %.2f\n", ((double)avr_tat/(double)numOfProcess));

    }
}


