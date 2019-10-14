#include <iostream> 
#include <string.h> 
#include <stdio.h>
#include<vector>
#include<fstream>
#include<mpi.h>

using namespace std; 


bool wPrefersM1OverM(vector< vector<int> > &prefer, int w, int m, int m1, int N) 
{ 
	for (int i = 0; i < N; i++) 
	{ 
		if (prefer[w][i] == m1) 
			return true; 

		if (prefer[w][i] == m) 
		return false; 
	} 
} 

int main() 
{ 

	MPI_Init(NULL, NULL);
	int rank, size;
	int m;
	int N;
	vector< vector<int> > prefer;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

	if(rank==0){
		cin>>N;
		for (int i = 0; i < 2*N; ++i)
		{
			vector<int> temp;
			for (int j = 0; j < N; ++j)
			{
				int val;
				cin>>val;
				if(i < N) val+=N;
				temp.push_back(val);
			}
			prefer.push_back(temp);
		}
		
	}
	int wPartner[N];
	int mPartner[N];
	ofstream log;
 	log.open ("Log.txt");

	bool mFree[N];

	memset(wPartner, -1, sizeof(wPartner));
	memset(mFree, false, sizeof(mFree)); 
	int freeCount = N;

	if(rank == 0){
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Send(&wPartner, 1, MPI_INT, 1,0,MPI_COMM_WORLD);
		MPI_Send(&mPartner, 1, MPI_INT, 1,0,MPI_COMM_WORLD);
		MPI_Send(&mFree, 1, MPI_INT, 1,0,MPI_COMM_WORLD);

		while (freeCount > 0){ 
			int m; 
			for (m = 0; m < N; m++) 
				if (mFree[m] == false) 
					break; 

			for (int i = 0; i < N && mFree[m] == false; i++) 
			{ 
				int w = prefer[m][i]; 

				if (wPartner[w-N] != -1) 
				{ 
					int m1 = wPartner[w-N]; 

					if (wPrefersM1OverM(prefer, w, m, m1, N) == false) 
					{ 
						wPartner[w-N] = m; 
						mPartner[m] = w-N;
						log << "Breaking man " << m1 << " with woman " << w-N << '\n';
						log << "Pairing man " << m << " with woman " << w-N << '\n';
						mFree[m] = true; 
						mFree[m1] = false; 
					} 
				} 
				else{ 
					wPartner[w-N] = m;
					mPartner[m] = w-N;
					log << "Pairing man " << m << " with woman " << w-N << '\n';
					mFree[m] = true; 
					freeCount--; 
				}
			}
		}
	}

	if(rank!=0){
		int t;
		MPI_Recv(&t, 1, MPI_INT, 0,0, MPI_COMM_WORLD, &status);
		int dist_size = N/size;
		int sub_wPartner[t];
		int sub_mPartner[t];
		bool sub_mFree[t];
		
		MPI_Recv(&sub_wPartner,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
		MPI_Recv(&sub_mPartner,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
		MPI_Recv(&sub_mFree,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
	}
	if(rank == 0){
	log.close();

	for (int i = 0; i < N; i++) 
		cout << i << " " << mPartner[i] << endl;
	}
	return 0;
} 