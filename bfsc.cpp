#include<iostream>
#include<vector>
#include<chrono>
#include<omp.h>
#include<stack>
#include<queue>

using namespace std;
using namespace std::chrono;

void DFS(vector<vector<int>> &matrix, int n, int start){
    vector<bool> visited(n,false);
    stack<int> s;
    visited[start] = true;
    s.push(start);

    while(!s.empty()){
        int node = s.top();
        s.pop();
        cout<<node<<" ";

        for(int i = 0; i<n; ++i){
            if(matrix[node][i] ==1 && !visited[i]){
                visited[i] = true;
                s.push(i);
            }
        }
    }
}

void BFS(vector<vector<int>> &matrix, int n, int start){
    vector<bool> visited(n,false);
    queue<int> q;
    visited[start] = true;
    q.push(start);

    while(!q.empty()){
        int node = q.front();
        q.pop();

        cout<<node<<" ";

        for(int i=0; i<n; ++i){
            if(matrix[node][i] == 1 && !visited[i]){
                visited[i] = true;
                q.push(i);
            }
        }
    }
}

// parallel BFS 

void parallel_bfs(const vector<vector<int>> &matrix, int n , int start){
    vector<bool> visited(n,false);
    queue<int> q;
    visited[start]  = true;
    q.push(start);

    #pragma omp parallel
    {
        #pragma omp single
        {
            while (!q.empty())
            {
                int node;
                #pragma omp critical
                {
                    if(!q.empty()){
                        node = q.front();
                        q.pop();
                    }
                    else{
                        node = -1;
                    }
                }

                if(node != -1){
                    cout<<node<<" ";

                    #pragma omp parallel for
                    {
                        for(int i = 0; i<n; ++i){
                            if(matrix[node][i] == 1 && !visited[i]){
                                #pragma omp critical
                                {
                                    if(!visited[i]){
                                        visited[i] = true;
                                        q.push(i);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
        }
    }
}

void parallel_dfs(const vector<vector<int>> & matrix, int n, int start){
    vector<bool> visited(n,false);
    stack<int> s;
    visited[start] = true;
    s.push(start);

    #pragma omp parallel
    {
        #pragma omp single
        {
            while(!s.empty()){
                int node;
                #pragma omp critical
                {
                    if(!s.empty()){
                        node = s.top();
                        s.pop();
                    }
                    else{
                        node = -1;
                    }
                }

                if(node != -1){
                    cout<<node<<" ";
                    #pragma omp parallel for
                    {
                        for(int i=0; i<n; ++i){
                            if(matrix[node][i] == 1 && !visited[i]){
                                #pragma omp critical
                                {
                                    if(!visited[i]){
                                        visited[i] = true;
                                        s.push(i);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void add(vector<vector<int>> &matrix, int u, int v){
    matrix[u][v] = 1;
    matrix[v][u] =1;
}

int main(){
    int n; 
    cout<<"Enter the number of elemnts: ";
    cin>>n;

    vector<vector<int>> matrix(n,vector<int>(n,0));

    for(int i=0; i<n; ++i){
        int u = rand() % 100;
        int v = rand() % 100;
        add(matrix,u,v);
    }

    int start = matrix[0][0];
    cout << "\nSequential BFS: \n";
    auto ss_bfs = high_resolution_clock::now();
    BFS(matrix,n,start);
    auto se_bfs = high_resolution_clock::now();
    double seq_bfs = duration<double,milli>(se_bfs - ss_bfs).count();
    cout << "\nTime Taken: " << seq_bfs << " ms\n\n";

     // Sequential DFS
     cout << "\nSequential DFS: \n";
     auto dfs_start = high_resolution_clock::now();
     DFS(matrix, n, start);
     auto dfs_end = high_resolution_clock::now();
     double seq_dfs = duration<double, milli>(dfs_end - dfs_start).count();
     cout << "\nTime Taken: " << seq_dfs << " ms\n\n";
 
     // Parallel BFS
     cout << "\nParallel BFS: \n";
     auto start_parallel_bfs = high_resolution_clock::now();
     parallel_bfs(matrix, n, start);
     auto end_parallel_bfs = high_resolution_clock::now();
     double par_bfs = duration<double, milli>(end_parallel_bfs - start_parallel_bfs).count();
     cout << "\nTime Taken: " << par_bfs << " ms";
     cout << "\nSpeedup Factor: " << seq_bfs / par_bfs << "\n\n";
 
     // Parallel DFS
     cout << "\nParallel DFS: \n";
     auto start_parallel_dfs = high_resolution_clock::now();
     parallel_dfs(matrix, n, start);
     auto end_parallel_dfs = high_resolution_clock::now();
     double par_dfs = duration<double, milli>(end_parallel_dfs - start_parallel_dfs).count();
     cout << "\nTime Taken: " << par_dfs << " ms";
     cout << "\nSpeedup Factor: " << seq_dfs / par_dfs << "\n\n";
 
     return 0;
}



