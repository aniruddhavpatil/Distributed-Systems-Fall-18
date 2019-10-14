#include<math.h>
#include <iostream>
#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <bits/stdc++.h>

using namespace std;

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

void DFS(int** graph, bool marked[], int n,
               int vx, int start, int &count, int v)
{
    marked[vx] = true;

    if (n == 0)
    {
        if(marked[vx]){
          marked[vx] = false;
        }
        if (!graph[vx][start])
            return;
        else{
            count++;
            return;
        }
    }
    int cnt = 0;
    while(cnt < v){
      if (graph[vx][cnt])
        if(!marked[cnt])
          DFS(graph, marked, n-1, cnt, start, count, v);
      cnt++;
    }

    // for (int i = 0; i < v; i++){
    //   if (graph[vx][i] && !marked[i]){
    //     DFS(graph, marked, n-1, i, start, count, v);
    //   }
    // }

    marked[vx] = false;
}

int main(int argc, char *argv[])
{
      MPI_Init(&argc, &argv);
      int rank, size, v, e;
      int n = 4;
      MPI_Status status;
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank != 0)
    {
        int **graph;
        MPI_Bcast(&e, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&v, 1, MPI_INT, 0, MPI_COMM_WORLD);
        graph = alloc_2d_int(v, v);
        int start_idx = (v/size) * rank;
        int end_idx = ((v - n - 1)/size) * (rank + 1);

        MPI_Recv(&(graph[0][0]), v*v, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        if(rank + 1 == size){
          int sum = v - n - 1;
          int increment = sum - end_idx;
          end_idx+=increment;
            // end_idx += (sum - end_idx);
          }


        bool marked[v];
        int n = 4;
        memset(marked, 0, sizeof(marked));

        int tempcount;
        int i=start_idx-1;
        while(++i < end_idx){
          marked[i] = true; 
          DFS(graph, marked, n-1, i, i, tempcount, v);
        }
        // for (int i = start_idx; i < end_idx; i++)
        // {
        //     DFS(graph, marked, n-1, i, i, tempcount, v);
        //     marked[i] = true;
        // }
        MPI_Send(&tempcount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
       cin>>v>>e;
       // scanf("%d %d", &v, &e);
       int edgelist[e][2];

       int **graph;
       graph = alloc_2d_int(v, v);

       int i=0;
       while(i < e){
           cin>>edgelist[i][0];
           cin>>edgelist[i][1];
           i++;
       }
       // for(int i = 0; i < e; i++)
           // scanf("%d %d", &edgelist[i][0], &edgelist[i][1]);

        for(int i = 0; i < v; i++)
        {
           for(int j = 0; j < v; j++)
              graph[i][j] = 0;
        }

        for(int i = 0; i < e; i++)
        {
           int v1 = edgelist[i][0];
           int v2 = edgelist[i][1];
           graph[v1][v2] = 1;
           graph[v2][v1] = 1;
        }

        MPI_Bcast(&e, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&v, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int key = 0;
        while(++key < size)
            MPI_Send(&(graph[0][0]), v*v, MPI_INT, key, 0, MPI_COMM_WORLD);


        // for(int key = 1; key < size; key++)
        //     MPI_Send(&(graph[0][0]), v*v, MPI_INT, key, 0, MPI_COMM_WORLD);

        int start_idx = (v/size) * rank;
        int total = v - n - 1;
        int equal_div = total/size;
        int end_idx = equal_div* (rank + 1);

        bool marked[v];
        memset(marked, 0, sizeof(marked));

        int count = 0;
        i = 0;
        while(i < v){
            DFS(graph, marked, n-1, i, i, count, v);
            marked[i++] = true;
        }
        // for (int i = 0; i < v; i++) {
        //     DFS(graph, marked, n-1, i, i, count, v);

        //     marked[i] = true;
        // }

        int sum = 0, tempcount = 0;
        for(int key = 1; key < size; key++)
        {
            MPI_Recv(&tempcount, 1, MPI_INT, key, 0, MPI_COMM_WORLD, &status);
            sum += tempcount;
        }

        int x = (count + sum) / 2;
        int count_checker = count/2;
        cout<<count_checker<<'\n';
        // printf("%d\n", count / 2);
    }

    MPI_Finalize();
    return 0;
}
