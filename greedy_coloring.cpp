#include <mpi.h>
#include <bits/stdc++.h>
#define PLACES 1000
#define MAIN 0
#define SET -1000
using namespace std;

vector<int> colors(PLACES, SET);
vector<int> adjList[PLACES];

vector<int> greedyColoring(vector<int>*  graph) {
	vector<int> result(PLACES, SET);

	int firstVertex = -1;
	while(++firstVertex<PLACES){
		if((int)graph[firstVertex].size() > 0) {
			break;
		}
	}
	bool available[PLACES];
	result[firstVertex]  = 0;

	int cr = -1;

	while(++cr < PLACES){
		available[cr] = false;
	}

	for (int u = firstVertex; u < PLACES; u++) {
		if(graph[u].size() > 0) {
			vector<int>::iterator i;
			i = graph[u].begin();
			while(i != graph[u].end()){
				int ind = result[*i];
				if(i == graph[u].begin()){
					i++;
					continue;
				}
				if (ind != -1)
					available[ind] = true;
				i++;
			}

			int cr;
			for (cr = 0; cr < PLACES; cr++)
				if (available[cr] == false)
					break;

			result[u] = cr;

			for (i = graph[u].begin(); i != graph[u].end(); ++i){
				int ind = result[*i];
				if (ind != -1)
					available[ind] = false;
			}

		}
	}
	return result;
}

vector<int> find_colored_vertices(vector<int> temp_colors) {
	vector<int> vert;
	int max_size = temp_colors.size();
	int i=0;
	while(i < max_size){
		if(temp_colors[i] != SET) {
			vert.push_back(i);
		}
		i++;
	}
	return vert;
}

void resolve_conflicts(vector<int>* temp_adjList, vector<int> & gbl_colors, vector<int> temp_colors, vector<int> colored_indices) {
	bool flag = true;
	vector<bool> bool_colors(PLACES+1, flag);

	for(int i=0; i<colored_indices.size(); i++) {
		for(int j=1; j<PLACES; j++) {
			int cii = colored_indices[i];
			if(temp_adjList[cii][j] != SET) {
				int temp_index = temp_adjList[cii][j];
				// cout<<temp_index<<endl;
				if(gbl_colors[temp_index] == temp_colors[cii]) {

					flag = false;

					for(int k=1; k<PLACES; k++) {
						int curr_neighbor = temp_adjList[cii][k];
						if(curr_neighbor == SET) {
							break;
						}
						if(gbl_colors[curr_neighbor] != SET) {
							int temp_index = gbl_colors[curr_neighbor];
							// cout<<temp_index<<endl;
							bool_colors[temp_index] = false;
						}
						else if(temp_colors[curr_neighbor] != SET) {
							int temp_index = gbl_colors[curr_neighbor];
							// cout<<temp_index<<endl;
							bool_colors[temp_colors[curr_neighbor]] = false;
						}
					}
					int idx = 0;
					while(idx < PLACES+1){
						if(bool_colors[idx])break;
						idx++;
					}
					gbl_colors[cii] = idx;
				}
			}
			else {
				break;
			}
		}
		for(int z=0; z<PLACES+1; z++) {
			bool_colors[z] = true;
		}

		if(flag){
			int cii = colored_indices[i];
			gbl_colors[cii] = temp_colors[cii];
		}
		if(!flag)
			flag = true;
		flag = true;
	}
}

void print_colors(vector<int> col) {
	vector<int> count(PLACES, 0);
	for(int i=0; i<PLACES; i++) {
		int col_id = col[i];
		if(col_id != SET) {
			count[col_id]++;
		}
	}
	int diffcolors = 0;

	for(int i=0; i<PLACES; i++) {
		if(count[i] != 0) {
			diffcolors++;
		}
	}
	cout << diffcolors << endl;

	for(int i=0; i<PLACES; i++) {
		int col_id = col[i];
		if(col_id != SET) {
			cout << col_id << endl;
		}
	}
}

int main(int argc, char **argv) {
	int world_size, world_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int tag_al = 3;
	int tag_vno = 2;
	int tag_color = 1;
	MPI_Status status;

	int vertex_chunks;

	if(world_rank == MAIN) {
		int vertices, edges;
		cin>>vertices>>edges;
		int i = -1;

		while(++i<vertices)
			adjList[i].push_back(i);

		int x, y;
		i = -1;
		while(++i < edges){
			cin>>x>>y;
			adjList[x].push_back(y);
			adjList[y].push_back(x);
		}

		i = -1;
		while(++i < vertices){
			int j = adjList[i].size() - 1;
			while(++j < PLACES)
				adjList[i].push_back(SET);
		}

		vertex_chunks = vertices / world_size;

		for(int i=0; i<vertices; i++) {
			colors[i] = SET;
		}

		int curr_vertex = 0;
		for(int dest = 1; dest < world_size; dest++) {

			MPI_Send(&vertex_chunks, 1, MPI_INT, dest, tag_vno, MPI_COMM_WORLD);
			int interm_size= curr_vertex+vertex_chunks;
			int i = curr_vertex - 1;
			while(++i < interm_size){
				MPI_Send(&adjList[i][0], PLACES, MPI_INT, dest, tag_al, MPI_COMM_WORLD);	
			}
			curr_vertex += vertex_chunks;
		}

		vector<int> MAIN_adjList[PLACES];

		for(int i=curr_vertex; i<vertices; i++) {
			MAIN_adjList[i] = adjList[i];
		}

		vector<int> temp_cols = greedyColoring(MAIN_adjList);

		resolve_conflicts(adjList, colors, temp_cols, find_colored_vertices(temp_cols));


		vector<int> temp_color(PLACES, SET);
		i = 0;
		while(++i < world_size){
			MPI_Recv(&temp_color[0], PLACES, MPI_INT, i, tag_color, MPI_COMM_WORLD, &status);
			resolve_conflicts(adjList, colors, temp_color, find_colored_vertices(temp_color));
		}
		print_colors(colors);
	}

	if(world_rank != MAIN) {

		vector<int> temp(1000, 88);
		MPI_Recv(&vertex_chunks, 1, MPI_INT, MAIN, tag_vno, MPI_COMM_WORLD, &status);

		int i = -1;
		while(++i < vertex_chunks){
			MPI_Recv(&temp[0], PLACES, MPI_INT, MAIN, tag_al, MPI_COMM_WORLD, &status);
			adjList[temp[0]] = temp;
		}

		vector<int> local_color = greedyColoring(adjList);

		MPI_Send(&local_color[0], PLACES, MPI_INT, MAIN, tag_color, MPI_COMM_WORLD);	

	}

	MPI_Finalize();

	return 0;
}
