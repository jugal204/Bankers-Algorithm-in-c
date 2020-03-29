#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
int R;
int P;
int *r;
int **a;
int **m;
int **n;
int *S;
int NP = 0;
int processes;
pthread_mutex_t lockRes;
pthread_cond_t condition;
bool getSafeSeq();
void* processCode(void* );
int main(int argc, char** argv) {
srand(time(NULL));
printf("\nEnter the number of processes ? ");
scanf("%d", &P);
printf("\nEnter the number of resources ? ");
scanf("%d", &R);
r = (int *)malloc(R * sizeof(*r));
printf("\nCurrently Available resources (R1 R2 ...)? ");
for(int i=0; i<R; i++)
scanf("%d", &r[i]);
a = (int **)malloc(P * sizeof(*a));
for(int i=0; i<P; i++)
a[i] = (int *)malloc(R * sizeof(**a));
m = (int **)malloc(P * sizeof(*m));
for(int i=0; i<P; i++)
m[i] = (int *)malloc(R * sizeof(**m));
printf("\n");
for(int i=0; i<P; i++) {
printf("\nResource allocated to process %d (R1 R2 ...)? ", i+1);
for(int j=0; j<R; j++)
scanf("%d", &a[i][j]);
}
printf("\n");
for(int i=0; i<P; i++) {
printf("\nMaximum resource required by process %d (R1 R2 ...)? ", i+1);
for(int j=0; j<R; j++)
scanf("%d", &m[i][j]);
}
printf("\n");
n = (int **)malloc(P * sizeof(*n));
for(int i=0; i<P; i++)
n[i] = (int *)malloc(R * sizeof(**n));
for(int i=0; i<P; i++)
for(int j=0; j<R; j++)
n[i][j] = m[i][j] - a[i][j];
S = (int *)malloc(P * sizeof(*S));
for(int i=0; i<P; i++) S[i] = -1;
if(!getSafeSeq()) {
printf("\nUnsafe State Detected! The processes leads the system to a unsafe state.\n\n");
exit(-1);
}
printf("\n\nSafe Sequence Found : ");
for(int i=0; i<P; i++) {
printf("%-3d", S[i]+1);
}
printf("\nExecuting Processes...\n\n");
sleep(1);
pthread_t processes[P];
pthread_attr_t attr;
pthread_attr_init(&attr);
int processNumber[P];
for(int i=0; i<P; i++) processNumber[i] = i;
for(int i=0; i<P; i++)
pthread_create(&processes[i], &attr, processCode, (void *)(&processNumber[i]));
for(int i=0; i<P; i++)
pthread_join(processes[i], NULL);
printf("\nAll Processes Finished\n");
free(r);
for(int i=0; i<P; i++) {
free(a[i]);
free(m[i]);
free(n[i]);
}
free(a);
free(m);
free(n);
free(S);
}
bool getSafeSeq() {
int tempRes[R];
for(int i=0; i<R; i++) tempRes[i] = r[i];
bool finished[P];
for(int i=0; i<P; i++) finished[i] = false;
int nfinished=0;
while(nfinished < P) {
bool safe = false;
for(int i=0; i<P; i++) {
if(!finished[i]) {
bool possible = true;
for(int j=0; j<R; j++)
if(n[i][j] > tempRes[j]) {
possible = false;
break;
}
if(possible) {
for(int j=0; j<R; j++)
tempRes[j] += a[i][j];
S[nfinished] = i;
finished[i] = true;
++nfinished;
safe = true;
}
}
}
if(!safe) {
for(int k=0; k<P; k++) S[k] = -1;
return false;
}
}
return true;
}
void* processCode(void *arg) {
int p = *((int *) arg);
pthread_mutex_lock(&lockRes);
while(p != S[NP])
pthread_cond_wait(&condition, &lockRes);
printf("\n--> Process %d", p+1);
printf("\n\tAllocated : ");
for(int i=0; i<R; i++)
printf("%3d", a[p][i]);
printf("\n\tNeeded    : ");
for(int i=0; i<R; i++)
printf("%3d", n[p][i]);
printf("\n\tAvailable : ");
for(int i=0; i<R; i++)
printf("%3d", r[i]);
printf("\n"); sleep(1);
printf("\tResource Allocated!");
printf("\n"); sleep(1);
printf("\tProcess Code Running...");
printf("\n"); sleep(rand()%3 + 2);
printf("\tProcess Code Completed...");
printf("\n"); sleep(1);
printf("\tProcess Releasing Resource...");
printf("\n"); sleep(1);
printf("\tResource Released!");
for(int i=0; i<R; i++)
r[i] += a[p][i];
printf("\n\tNow Available : ");
for(int i=0; i<R; i++)
printf("%3d", r[i]);
printf("\n\n");
sleep(1);
NP++;
pthread_cond_broadcast(&condition);
pthread_mutex_unlock(&lockRes);
pthread_exit(NULL);
}
