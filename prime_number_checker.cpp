#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int rank, size, number = atoi(argv[1]);

    if(MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        fprintf(stderr, "Unable to initialize MPI!\n");
        return -1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ARRAY_SIZE;
    ARRAY_SIZE = number - 2;

    MPI_Status status;
    int ARRAY_CHUNK_SIZE = ARRAY_SIZE/size;

    int total_sum = 0;
    int * array = (int *) malloc(sizeof(int) * ARRAY_SIZE);
    int * chunk = (int *) malloc(sizeof(int) * ARRAY_CHUNK_SIZE);
    int i;

    for(i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = i + 2;
    }

    MPI_Scatter(array,
        ARRAY_CHUNK_SIZE,
        MPI_INT,
        chunk,
        ARRAY_CHUNK_SIZE,
        MPI_INT,
        0,
        MPI_COMM_WORLD);

   int key = 1, temp = 0, primeflag = 1, sum = 0;

    for(i = 0; i < ARRAY_CHUNK_SIZE; i++)
    {
        int remainder = number % chunk[i];
        if(remainder == 0) primeflag = remainder;
    }

    bool rank_zero = false;
    bool prime_one = false;

    if(rank == 0) rank_zero = true;
    if(prime_one == 1) prime_one = true;

    if(!rank_zero) MPI_Send(
        &primeflag, 
        1, 
        MPI_INT, 
        0, 
        0, 
        MPI_COMM_WORLD);

    else if(rank_zero && prime_one)
    {
        for(key = 1; key < size; key++)
        {
            int ifprime;
            ifprime = 1;

            MPI_Recv(
                &ifprime, 
                1, 
                MPI_INT, 
                key, 
                0, 
                MPI_COMM_WORLD, 
                &status);

            if(ifprime == 0)
            {
                primeflag = 0;
                break;
            }
        }
    }

    if(rank_zero)
        if(primeflag==0) printf("%d is not a prime\n",number);
        else printf("%d is a prime\n",number);

    free(array);
    free(chunk);
    MPI_Finalize();
    return 0;

}
