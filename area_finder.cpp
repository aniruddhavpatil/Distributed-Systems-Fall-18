#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<iostream>

using namespace std;
int main(int argc, char *argv[])
{
    int n, i, i_ind, j_ind, rank, size;
    int key = 1;
    // cin>>n;
    int temp = n;
    int *array = (int *) malloc(sizeof(int) * n);

    // i=0;
    // while(temp--){
    //     int x,y;
    //     cin>>x>>y;
    //     X[i] = x;
    //     Y[i] = y;
    //     i++;
    //     cout<<temp<<endl;
    // }
    n = 4;


    double X[] = {0, 2, 2, 0};
    bool proceed = true;
    double Y[] = {2, 2, 0, 0};

    if(MPI_Init(&argc, &argv) != MPI_SUCCESS) proceed = false;

    if(proceed == false)
    {
        fprintf(stderr, "Unable to initialize MPI!\n");
        return -1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for(i = 0; i < n; i++) array[i] = i;

    int chunk_size = n/size;
    int * chunk = (int *) malloc(sizeof(int) * chunk_size);
    MPI_Status status;

    MPI_Scatter(
        array,
        chunk_size, 
        MPI_INT, 
        chunk, 
        chunk_size, 
        MPI_INT, 
        0, 
        MPI_COMM_WORLD);

    double area_new;
    double area = 0.0;

    for(i=0; i < chunk_size; i++)
    {
      i_ind = chunk[i];
      j_ind = (i_ind - 1) % n;

      double temp;
      double xi,xj,yi,yj;

      xi = X[i_ind];
      xj = X[j_ind];

      yi = Y[i_ind];
      yj = Y[j_ind];

      temp = (xj + xi) * (yj - yi);
      area += temp;
    }
    area *= 0.5;

    if(rank != 0) MPI_Send(
        &area,
        1,
        MPI_DOUBLE,
        0,
        0,
        MPI_COMM_WORLD);

    else if(rank == 0)
    {
        for(key = 1; key < size; key++)
        {
            MPI_Recv(
                &area_new,
                1,
                MPI_DOUBLE,
                key,
                0,
                MPI_COMM_WORLD,
                &status);
            area =  area + area_new;
        }
    }

    if(rank == 0) printf("%lf\n", area);
}
