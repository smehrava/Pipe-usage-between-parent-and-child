#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int summation(int start, int end)
{
	int sum = 0;
	if (start < end)
	{
		sum = ((end * (end + 1)) - (start * (start - 1))) / 2;
	}
	return sum;
}

int ith_part_start(int i, int N, int M)
{
	int part_size = N / M;
	int start = i * part_size;
	return start;
}
int ith_part_end(int i, int N, int M)
{
	int part_size = N / M;
	int end = (i < M - 1) ? ((i + 1) * part_size - 1) : N;
	return end;
}
int main(int argc, char **argv)
{
    //first arguement written by user is going N and second is going to M
	int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int pipes[M][2];
    pid_t parent,pid1,grandpid;

    parent = getpid();

    printf("parent(PID %d): process started\n\n", parent);

    

    pid1 = fork();

    if (pid1 == 0){  //child
        printf("\n\n");

        printf("child_1(PID %d): process started from parent(PID %d)\n",getpid(),parent);

        printf("child_1(PID %d): forking child_1.1....child_1.%d\n\n",getpid(),M);

        for (int i=0; i< M;i++){

            pipe(pipes[i]);
            //child1 forkiing its children meaning grandchildren of parent.
            grandpid = fork();

            if (grandpid == 0){
                printf("child_1.%d(PID %d): fork() successful\n", i+1 ,getpid());
                //calling the functions given to us to get the sum of numbers between start and end.
                int start = ith_part_start(i,N,M);
                int end = ith_part_end(i,N,M);
                int sum = summation(start,end);

                printf("child_1.%d(PID %d): partial sum: [%d - %d] = %d\n",i+1, getpid(),start,end,sum);
                // closing reading side of pipes cause if we dont, we are going to end up repeating reading for more than one time.
                close(pipes[i][0]);
                //writing all the sums we got from calling functions ith-part-start, ith-partp-end and summation inside pipes.
                write(pipes[i][1], &sum , sizeof(sum));

                //closing the writing part of pipes because of the same reason we closed the reading part.
                close(pipes[i][1]);

                //using this exit function to avoid repeatation 
                exit(0);

            }
        }

        //child_1 has waited for all it's children to execute and now its time to find out total sum.
        wait(NULL);
        int total_sum = 0;
        for (int i =0;i < M;i++){
            int partial_sum;
            //closing the writing part of the pipe because it's still open
            close(pipes[i][1]);
            
            //now we need to read all the data given to pipes from position [0] cause 0 is always for reading and 1 for writing.
            //we read all the data in order to figure whats the total sum
            read(pipes[i][0], &partial_sum, sizeof(partial_sum));

            //closing the reading part of the pipe.
            close(pipes[i][0]);

            //adding partial sums together to find the ultimate total_sum
            total_sum = total_sum + partial_sum;

            
        }

        printf("\nchild_1(PID %d): total sum = %d\n",getpid(), total_sum);

        printf("child_1(PID %d): child_1 completed\n",getpid());
    }


    else if (pid1> 0){  
        //parent
        printf("parent(PID %d): forking child_1\n",parent);
        printf("parent(PID %d): fork successful for child_1(PID %d)\n",parent, pid1);
        printf("parent(PID %d): waiting for child_1(PID %d) to complete\n",parent, pid1);

        //waiting for child 1 to execute completely
        wait(NULL);

        printf("\nparent(PID %d): parent completed\n",parent);

    }

    return 0;


}