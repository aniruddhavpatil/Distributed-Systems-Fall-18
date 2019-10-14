#include<time.h>
#include<stdbool.h>
#include<mpi.h>
#include<bits/stdc++.h>
#include<stdio.h> 
#include<limits.h> 
using namespace std;

  
int minKey(int key[], bool mstSet[], int V) {
int min = INT_MAX;
int min_index; 
  
for (int v = 0; v < V; v++) 
    if (mstSet[v] == false)
        if(!(key[v] >= min)) {
            min = key[v];
           min_index = v; 
        }
  
return min_index; 
} 
  
int printMST(int parent[], int V, vector<vector <int> > graph) 
{ 
    int sum = 0;
    for (int i = 1; i < V; i++){
        int p = parent[i];
        int w = graph[i][p];
        sum += w;

    }

    cout << sum << endl;
    for (int i = 1; i< V; i++)
        cout<< parent[i] <<' '<< i << ' ' << graph[i][parent[i]]<<'\n';
}
  
void util(int x, int signal){
    int sum = 0;
    exit(sum);
}

void primMST(vector<vector<int> > graph) 
{ 
    int V = graph[0].size();

    int parent[V];
    memset(parent, 0, sizeof(parent));
    int key[V];  
    memset(key, 0, sizeof(key));
    bool mstSet[V];  
    memset(mstSet, false, sizeof(mstSet));

    for (int i = 0; i < V; i++) 
        key[i] = INT_MAX, mstSet[i] = false; 
  
    parent[0] = -1;
    key[0] = 0;      
  
    for (int count = 0; count < V-1; count++) 
    { 
        int u = minKey(key, mstSet, V); 
        mstSet[u] = true; 
        int v = 0;
        while(v<V){
            if (graph[u][v])
                if(mstSet[v] == false)
                    if(graph[u][v] < key[v]) 
                        parent[v] = u, key[v] = graph[u][v]; 
            v++;
        }
    } 
  
    printMST(parent, V, graph); 
} 
  
  
int main() 
{
    int rank, size;
    int n, m;
    vector<vector<int> > graph;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    if(rank == 0){
        cin >> n >> m;
        graph.resize(n);
        for(int i = 0;i < n;i++){
            graph[i].resize(n);
        }
        for(int i = 0 ; i < m; i++)
        {   
            int a, b, w;
            cin>> a >> b >> w;
            graph[b][a] = w;
            graph[a][b] = graph[b][a];
        }
        primMST(graph);
        MPI_Send(&graph,1,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    if(rank != 0){
        int x;
        int dist_size = n/size;
        MPI_Recv(&x,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        vector<vector<int> > sub_array(n);
        util(x,dist_size);
        MPI_Recv(&sub_array,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
    }
    MPI_Finalize();
    return 0; 
}
