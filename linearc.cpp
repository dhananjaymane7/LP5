#include<iostream>
#include<chrono>
#include<omp.h>

#define N 5
#define RUNS 100000
using namespace std;
using namespace std::chrono;

void linear_seq(float* x, float* y, int n , float &w, float& b){
    float sumxx =0, sumxy=0, sum_x=0, sum_y=0;

    for(int i =0; i<n; ++i){
        sum_x += x[i];
        sum_y += y[i];
        sumxx += x[i] * x[i];
        sumxy += x[i] * y[i];
    }

    w = (n * sumxy - sum_x * sum_y)/(n * sumxx - sum_x * sum_x);
    b= (sum_y - w * sum_x)/n;
}

void linear_parallel(float* x, float*y, int n, float &w, float &b){
    float sumx=0, sumy=0, sumxx=0, sumxy=0;

    #pragma omp parallel for reduction(+:sumx,sumy,sumxx,sumxy)
    for (int i = 0; i < n; ++i)
    {
        sumx += x[i];
        sumy += y[i];
        sumxx += x[i] * x[i];
        sumxy += x[i] * y[i];
    }

    w = (n * sumxy - sumx * sumy)/(n * sumxx - sumx * sumx);
    b = (sumy - w * sumx)/n;
    
}


void run_test(float x[N], float y[N], int case_num){
    float w_seq =0, b_seq =0, w_omp =0, b_omp=0;

    auto s_seq = high_resolution_clock::now();
    for (int i = 0; i < RUNS; ++i)
    {
        linear_seq(x,y,N,w_seq,b_seq);
    }
    auto e_seq = high_resolution_clock::now();
    double duration_seq = duration<double,milli>(e_seq - s_seq).count();

    auto start_omp = high_resolution_clock::now();
    for (int i = 0; i < RUNS; ++i)
        linear_parallel(x, y, N, w_omp, b_omp);
    auto end_omp = high_resolution_clock::now();
    double duration_omp = duration<double,milli>(end_omp - start_omp).count();
    
    double avg_seq_time = (duration_seq) / RUNS;
    double avg_omp_time = (duration_omp) / RUNS;

    cout << "\n=== Test Case " << case_num << " ===" << endl;
    cout << "Average Time (Sequential): " << avg_seq_time << " milliseconds" << endl;
    cout << "Average Time (OpenMP):     " << avg_omp_time << " milliseconds" << endl;

    if (avg_omp_time > 0)
        cout << "Speedup:                   " << avg_seq_time / avg_omp_time << "x" << endl;
    else
        cout << "Speedup:                   N/A (too fast to measure accurately)" << endl;

    cout << "Equation from Sequential: y = " << w_seq << " * x + " << b_seq << endl;
    cout << "Equation from OpenMP:     y = " << w_omp << " * x + " << b_omp << endl;

}

int main(){
     // Test Case 1: y = 2x + 0.1
     float x1[N] = {1, 2, 3, 4, 5};
     float y1[N] = {2.1, 4.1, 6.1, 8.1, 10.1};
 
     // Test Case 2: y = -3x + 5
     float x2[N] = {1, 2, 3, 4, 5};
     float y2[N] = {2, -1, -4, -7, -10};
 
     // Test Case 3: y = 0.5x + 2
     float x3[N] = {1, 2, 3, 4, 5};
     float y3[N] = {2.5, 3.0, 3.5, 4.0, 4.5};
 
     run_test(x1, y1, 1);
     run_test(x2, y2, 2);
     run_test(x3, y3, 3);
 
     return 0;
}