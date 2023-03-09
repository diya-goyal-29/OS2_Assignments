//============================================================================
// Name        : SrcAssign4_tas_cs20btech11014.cpp
// Author      : Diya Goyal
//============================================================================

#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <atomic>
#include <random>
using namespace std;

int n = 0, k = 0, lambda1 = 0, lambda2 = 0, total_waiting_time = 0;
double t1, t2;

atomic_int lock;


time_t getSysTime() {
	time_t _time = time(NULL);
	return _time;
}

int compare_and_swap(atomic_int *value, int expected, int new_value) {
	int temp = *value;
	if (*value == expected)
		*value = new_value;
	return temp;
}

void entry_sec() {
	while (true)
	        {
	            int expected = 0;
	            if (lock.compare_exchange_weak(expected, 1))
	                break;
	        }
}

void exit_sec() {
	lock.store(0);
}

void testCS(int id) {

	default_random_engine generator;
	exponential_distribution<double> distribution1((double)1/lambda1);
	t1 = distribution1(generator);
	exponential_distribution<double> distribution2((double)1/lambda2);
	t2 = distribution2(generator);

	for (int i = 0; i < k; i++) {
		time_t requiredEnterTime = getSysTime();
		cout << i + 1 << "th CS Request at " << ctime(&requiredEnterTime) << " by thread " << id << "\n";
		entry_sec();
		time_t actualEnterTime = getSysTime();
		cout << i + 1 << "th CS Entry at " << ctime(&actualEnterTime) << " by thread " << id << "\n";
		sleep(t1);
		exit_sec();
		time_t exitTime = getSysTime();
		cout << i + 1 << "th CS Exit at " << ctime(&exitTime) << " by thread " << id << "\n";
		sleep(t2);
		total_waiting_time += (actualEnterTime - requiredEnterTime);
	}
}

int main() {

	ifstream fptr("input_sample.txt");
//	fptr.open("input_sample.txt");
	fptr >> n >> k >> lambda1 >> lambda2;
	fptr.close();

	thread tid[n];
	lock = 0;

		for(int i = 0; i < n; i++){
			tid[i] = thread(testCS, (i+1));
		}

		for(int i = 0; i < n; i++) {
			tid[i].join();
		}

	return 0;
}
