#include<mpi.h>
#include<stdio.h>
#include <iostream>
#include<string.h>
#include<math.h>
#include<stdlib.h>
using namespace std;



int count_kwalks(int **graph, int u, int v, int k, int V, int start_idx, int end_idx)
{
   if (k <= 0)
      return 0;

   if (k == 0) 
    if(u == v)
      return 1;

   if(k == 1)
    if(graph[u][v])
      return 1;

   int count = 0;

   int i = start_idx-1;

   while(++i < end_idx){
       if (graph[u][i] == 1)
           count += count_kwalks(graph, i, v, k-1, V, 0, V);
   }
   // for (int i = start_idx; i < end_idx; i++)
   //     if (graph[u][i] == 1)
   //         count += count_kwalks(graph, i, v, k-1, V, 0, V);

   return count;
}

int **alloc_2d_int(int rows, int cols) {
    int **array= (int **)malloc(rows*sizeof(int*));
    int *temp = (int *)malloc(rows*sizeof(int));
    int *data = (int *)malloc(rows*cols*sizeof(int));
    for (int i=0; i<rows; i++){
        temp[i]-=1;
        temp[i]+= data[i];
        array[i] = &(data[cols*i]);
        temp[i]+= data[i]/2;
      }
    free(temp);
    return array;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int rank, size, v, e, k;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank != 0)
    {
        MPI_Bcast(&v, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int **graph;
        graph = alloc_2d_int(v, v);
        MPI_Bcast(&e, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Recv(&(graph[0][0]), v*v, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int v_d = v/size;
        int start_idx = v_d * rank;
        int end_idx = v_d * (rank + 1);

        if(rank + 1 == size )
            end_idx += (v - end_idx);

        int a = 0;
        int b = v - 1;
        int temp;
        temp = count_kwalks(graph, a, b, k, v, start_idx, end_idx);
        MPI_Send(&temp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }
    else
    {
      cin>>v>>e>>k;
        int **graph;
        int *edgelist[e];
        int i = 0;
        while(i < e)
          edgelist[i++] = new int[2];

        graph = alloc_2d_int(v, v);

        i=0;
        while(i < e){
            cin>>edgelist[i][0];
            cin>>edgelist[i][1];
            i++;
          }

         for(int i = 0; i < v; i++)
            memset(graph[i], 0, sizeof(graph[i]));

         for(int i = 0; i < e; i++)
         {
            int v1 = edgelist[i][0];
            int v2 = edgelist[i][1];
            graph[v1 - 1][v2 - 1] = 1;
            graph[v2 - 1][v1 - 1] = 1;
         }

        MPI_Bcast(&v, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&e, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int key = 0;
        while(++key < size)
           MPI_Send(&(graph[0][0]), v*v, MPI_INT, key, 0, MPI_COMM_WORLD);

        int v_d = v/size;
        int start_idx = v_d * rank;
        int end_idx = v_d * (rank + 1);

        int a = 0;
        int b = v - 1;

        int temp = 0;
        int count = count_kwalks(graph, a, b, k, v, start_idx, end_idx);

        key = 0;
        while(++key < size)
        {
           MPI_Recv(&temp, 1, MPI_INT, key, 0, MPI_COMM_WORLD, &status);
           count = count + temp;
        }
        cout << count << endl;
    }

   MPI_Finalize();
	return 0;
}
