#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <mpi.h>
#include <vector>

using namespace std;

void swap(int *v, int i, int j);
int * merge(int *A, int asize, int *B, int bsize);
void m_sort(int *A, int min, int max);

void swap(int *v, int i, int j)
{
	int t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}

int * merge(int *A, int asize, int *B, int bsize) {
	int ai = 0, bi = 0, ci= 0, i;
	int *C;
	int csize;
	csize = asize+bsize;

	C = (int *)malloc(csize*sizeof(int));
	bool a_comp = false, b_comp = false;
	if(ai < asize) a_comp = true;
	if(bi < bsize) b_comp = true;

	while (a_comp && b_comp) {
		if (A[ai] <= B[bi]) {
			C[ci++] = A[ai++];
		} else {
			C[ci++] = B[bi++];
		}
	}

	if (!a_comp){
		i = ci;
		while(i < csize){
			C[i++] = B[bi++];
		}
	}
	else if (!b_comp){
		i = ci;
		while(i < csize){
			C[i++] = A[ai++];
		}
	}
	i = 0;
	while(i<asize){
		A[i] = C[i];
		i++;
	}

	i = 0;
	while(i<bsize){
		B[i] = C[asize+i];
		i++;
	}
	return C;
}



void m_sort(int *A, int min, int max)
{
	
	int mid = (min+max)/2;
	int upperCount = max - mid;
	int lowerCount = mid - min + 1;
	int *C;
	if (!(max == min)) {
		m_sort(A, mid+1, max);
		m_sort(A, min, mid);
		C = merge(A + min, lowerCount, A + mid + 1, upperCount);
		
	} else return;
}

void trace(int *chunk, int s, int id){
	cout << "Sending chunk [";
	for(int i = 0;i<s;i++){
		cout << chunk[i] << " ";
	}
	cout << "] to process " << id << endl;
	return;
}

void trace_to(int *chunk, int s, int id, int to){
	cout << "Sent chunk [";
	for(int i = 0;i<s;i++){
		cout << chunk[i] << " ";
	}
	cout << "] from process " << id << " to process " << to << endl;
	return;
}

int main(int argc, char **argv)
{
	int * data;
	int * chunk;
	vector<int> data_input;
	int * other;
	int m,n,id,p, s = 0,i,x, step;

	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	if(id==0)
	{
		string line;
    	getline(cin, line);
    	istringstream iss(line);
    	while(iss >> x) {
      		data_input.push_back(x);
    	}
    	n = data_input.size();
		int r;
		r = n%p;
		s = n/p;
		int data_number = n+s-r;
		data = (int *)malloc(data_number*sizeof(int));
		i=0;
		while(i<n){
			data[i] = data_input[i];
			i++;
		}

		if(r!=0)
		{
			i=n;
			while(i<data_number) data[i++]=0;
			s=s+1;
		}

		MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
		chunk = (int *)malloc(s*sizeof(int));
		MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);
		trace(chunk,s,id);
		m_sort(chunk, 0, s-1);
	}
	else
	{
		MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
		chunk = (int *)malloc(s*sizeof(int));
		MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);
		trace(chunk,s,id);
		m_sort(chunk, 0, s-1);
	}

	step = 1;
	while(step<p)
	{
		int step_counter = id%(2*step);
		if(step_counter==0)
		{
			int bound = id + step;
			if(! bound >= p)
			{
				int prev_m = m;
				MPI_Recv(&m,1,MPI_INT,id+step,0,MPI_COMM_WORLD,&status);
				int curr_m = m;
				bool check_change = false;
				if(prev_m != curr_m) check_change = true;
				other = (int *)malloc(m*sizeof(int));
				prev_m = m;
				MPI_Recv(other,m,MPI_INT,id+step,0,MPI_COMM_WORLD,&status);
				curr_m = m;
				if(prev_m != curr_m) check_change = true;
				chunk = merge(chunk,s,other,m);
				s = s+m;
			}
		}
		else
		{
			int near = id-step;
			MPI_Send(&s,1,MPI_INT,near,0,MPI_COMM_WORLD);
			MPI_Send(chunk,s,MPI_INT,near,0,MPI_COMM_WORLD);
			trace_to(chunk,s,id,near);
			break;
		}
		step = step*2;
	}

	if(id==0)
	{
		FILE * fout;

		fout = fopen("result","w");
		for(i=0;i<n;i++)
			fprintf(fout,"%d\n",chunk[i]);
		fclose(fout);
	}
	MPI_Finalize();
	return 0;
}
