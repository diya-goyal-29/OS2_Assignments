#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <random>
#include <semaphore.h>

using namespace std;

int nr, kr, nw, kw, mu_cs, mu_rem;
sem_t rw_mutex;
sem_t mutex;
sem_t queue;
int read_count = 0;
float total_waiting_time_r = 0, max_waiting_time_r = 0, total_waiting_time_w = 0, max_waiting_time_w = 0;
double randCSTime, randRemTime;

ofstream output_log("FairRW_log.txt"),output_average("Average_time.txt");

time_t getSysTime() {
	time_t _time = time(NULL);
	return _time;
}

void writer(int id) {
	default_random_engine generator;
	exponential_distribution<double> distribution1(mu_cs);
	randCSTime = distribution1(generator);
	exponential_distribution<double> distribution2(mu_rem);
	randRemTime = distribution2(generator);
	for (int i = 1; i < kw; i++) {
		time_t reqTime = getSysTime();
		output_log << i << "th CS request by Writer Thread " << id << " at " << ctime(&reqTime) << endl;
		//Entry Section
		sem_wait(&queue);
		sem_wait(&rw_mutex);
		sem_post(&queue);
		time_t enterTime = getSysTime();
		output_log << i << "th CS entry by Writer Thread " << id << " at " << ctime(&enterTime) << endl;
		//Critical Section
		sleep(randCSTime);
		//Exit Section
		sem_post(&rw_mutex);
		time_t exitTime = getSysTime();
		output_log << i << "th CS exit by Writer Thread " << id << " at " << ctime(&exitTime) << endl;
		//Remainder Section
		sleep(randRemTime);
		total_waiting_time_w += (enterTime - reqTime);
		if(max_waiting_time_w < (enterTime - reqTime)) {
			max_waiting_time_w = (enterTime - reqTime);
		}
	}
}

void reader(int id) {
	default_random_engine generator;
	exponential_distribution<double> distribution1(mu_cs);
	randCSTime = distribution1(generator);
	exponential_distribution<double> distribution2(mu_rem);
	randRemTime = distribution2(generator);
	for (int i = 1; i < kr; i++) {
		time_t reqTime = getSysTime();
		output_log << i << "th CS request by Reader Thread " << id << " at " << ctime(&reqTime) << endl;
		//Entry Section
		sem_wait(&queue);
		sem_wait(&mutex);
		read_count++;
		if (read_count == 1)
			sem_wait(&rw_mutex);
		sem_post(&queue);
		sem_post(&mutex);
		time_t enterTime = getSysTime();
		output_log << i << "th CS entry by Reader Thread " << id << " at " << ctime(&enterTime) << endl;
		//Critical Section
		sleep(randCSTime);
		//Exit Section
		sem_wait(&mutex);
		read_count--;
		if (read_count == 0)
			sem_post(&rw_mutex);
		sem_post(&mutex);
		time_t exitTime = getSysTime();
		//Remainder Section
		output_log << i << "th CS exit by Reader Thread " << id << " at " << ctime(&exitTime) << endl;
		sleep(randRemTime);
		total_waiting_time_r += (enterTime - reqTime);
		if(max_waiting_time_r < (enterTime - reqTime)) {
			max_waiting_time_r = (enterTime - reqTime);
		}
	}
}

int main() {

	ifstream fptr("input_sample.txt");
	fptr >> nw >> nr >> kw >> kr >> mu_cs >> mu_rem;
	fptr.close();
	
	sem_init(&rw_mutex,0,1);
	sem_init(&mutex,0,1);
	sem_init(&queue,0,1);
	
	thread tid_r[nr];
	thread tid_w[nw];
	
	for(int i = 0; i < nr; i++) {
		tid_r[i] = thread(reader, i+1);
	}
	
	for(int i = 0; i < nw; i++) {
		tid_w[i] = thread(writer, i+1);
	}
	
	for(int i = 0; i < nr; i++) {
		tid_r[i].join();
	}
	for(int i = 0; i < nw; i++) {
		tid_w[i].join();
	}
	
	output_average << "\n\nAverage waiting time for reader thread : " << total_waiting_time_r/(nr*kr) << "\nMaximum waiting time for reader thread : " << max_waiting_time_r << "\nAverage waiting time for writer thread : " << total_waiting_time_w/(nw*kw) << "\nMaximum waiting time for writer thread : " << max_waiting_time_w << endl;

	return 0;
}
