#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"
 
#define RESERVE 200
#define FIELD 1
#define START 2
#define WORK 3
#define END_OF_WORK 4
#define ACCIDENT 5
#define FILL 1000
 
int LOAD=1;
int NOT_LOAD = 0;
int number_of_processes;
int number_process;
int amount_tractors;
int number_of_tractor;
int amount_fields=8;
int amount_of_fields_sown = 0;
int fertilizers = 3000;
int t=1;
int leave[2];
int load_it[2];
MPI_Status st;
void Go(int number_of_tr, int state) 
{
leave[0] = number_of_tr;
leave[1] = state;
MPI_Send(&leave, 2, MPI_INT, 0, t, MPI_COMM_WORLD);
sleep(1);
}
void Field(int amount_of_pr){
int number_of_tractor, status;
amount_tractors = number_of_processes - 1;
if(rand()%2==1){
printf("Good weather, go\n");
}
else{
printf("It's raining\n");
}
printf("You have to sow %d fields\n", amount_fields);
sleep(2);
while(amount_fields <= amount_tractors){
MPI_Recv(&load_it,2,MPI_INT,MPI_ANY_SOURCE,t,MPI_COMM_WORLD, &st);
number_of_tractor=load_it[0];
status=load_it[1];
if(status==1){
printf("Tractor %d is on field\n", number_of_tractor);
}
if(status==2){
printf("The fertilizers are in seeder tractor %d should go to field nr %d\n", number_of_tractor, amount_of_fields_sown);
amount_of_fields_sown--;
}
if(status==3){
printf("Tractor %d is sowing\n", number_of_tractor);
}
if(status==4){
if(amount_of_fields_sown < amount_fields){
amount_of_fields_sown++;
MPI_Send(&LOAD, 1, MPI_INT, number_of_tractor, t, MPI_COMM_WORLD);
}
else{
MPI_Send(&NOT_LOAD, 1, MPI_INT, number_of_tractor, t, MPI_COMM_WORLD);
}
}
if(status==5){
amount_tractors--;
printf("Amount tractors %d\n", amount_tractors);
}
} 
printf("Program finished\n");
}

void Tractor()
{
int state;
int sum;
int i;
state = WORK; 
while(1){
if(state == 1){
if(rand()%2 == 1){
state = START;
fertilizers = FILL;
printf("Leave, tractor %d\n",number_process);
Go(number_process,state);
}
else{
Go(number_process,state);
}
}
else if(state==2){
printf("I leave, tractor %d\n",number_process);
state = WORK;
Go(number_process,state);
}
else if(state==3){
fertilizers-=rand()%500; 
if(fertilizers<=RESERVE){
state=END_OF_WORK;
printf("Fill tank\n");
Go(number_process,state);
}
 
else{
for(i=0; rand()%10000;i++);
}
}
else if(state==4){
int tmp;
MPI_Recv(&tmp, 1, MPI_INT, 0, t, MPI_COMM_WORLD, &st);
if(tmp==LOAD){
state = FIELD;
printf("Return, tractor %d\n", number_process);
}
else
{
fertilizers-=rand()%500;
if(fertilizers>0){
Go(number_process,state);
}
else{
state=ACCIDENT;
printf("Damaged\n");
Go(number_process,state);
return;}}}}}
int main(int argc, char *argv[])
{
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD,&number_process);
MPI_Comm_size(MPI_COMM_WORLD,&number_of_processes);
srand(time(NULL));
if(number_process == 0)
Field(number_of_processes);
else 
Tractor();
MPI_Finalize();
return 0;
}
