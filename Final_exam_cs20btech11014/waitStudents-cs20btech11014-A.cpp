//============================================================================
// Name        : final_exam.cpp.cpp
// Author      : Diya Goyal
//============================================================================

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

struct student {
	int id;
	int subject;
};

int k, m, lambda, n, mu, subject, total_waiting_time = 0;
int lab_count = 0;
sem_t mutex;
sem_t subject_mutex;
sem_t lab_capacity;
ofstream output_log("LogFile-cs20btech11014-A.txt");

time_t getSysTime() {
	time_t _time = time(NULL);
	return _time;
}

void student_func(struct student *s, int subject) {
	default_random_engine generator;
	exponential_distribution<double> distribution1(mu);
	double randCSTime = distribution1(generator);
	time_t reqTime = getSysTime();
	output_log << "Student "<< s->id << "of group k" << s->subject << " requested the lab at " << ctime(&reqTime) << endl;
	
	sem_wait(&mutex);
	sem_wait(&lab_capacity);
		if(lab_count <= m) {
			sem_post(&lab_capacity);
			}
			
			if(s->subject != subject){
				sem_wait(&subject_mutex);
				//cout<<s->id<<" "<<s->subject<<" "<<subject<<endl;
				subject = s->subject;
				//cout<<subject<<endl;
			}
			lab_count++;
			sem_post(&mutex);
		
		time_t enterTime = getSysTime();
		output_log << "Student "<< s->id << "of group k" << s->subject << " entered the lab at " << ctime(&reqTime) << endl;
		
		sleep(randCSTime);
		sem_wait(&mutex);
		lab_count--;
		if (lab_count == 0)
			sem_post(&subject_mutex);				
		sem_post(&mutex);
		total_waiting_time += enterTime - reqTime;
		time_t exitTime = getSysTime();
		output_log << "Student "<< s->id << "of group k" << s->subject << " exited the lab at " << ctime(&reqTime) << endl;
		
}

int main() {

	int number_of_students_entering = 0;
	
	ifstream fptr("input_file.txt");
	fptr >> k >> m >> lambda >> n >> mu;
	fptr.close();
	
	sem_init(&subject_mutex,0,1);
	sem_init(&mutex,0,1);
	sem_init(&lab_capacity,0,1);
	
	struct student s[n];
	thread tid_s[n];
	
	for(int i = 0; i < n; i++) {
		s[i].id = i+1;
		s[i].subject = (rand() % k) + 1;
		//cout<<s[i].id<<" "<<s[i].subject<<endl;
	}
	subject = s[0].subject;
	default_random_engine generator;
	exponential_distribution<double> distribution(lambda);
	double num_students = distribution(generator);
	//cout<<num_students<<endl;
	for(int i = 0; i < n;) {
		for(int j = 0; j < num_students; j++) {
			if(number_of_students_entering > n) 
				continue;
			tid_s[number_of_students_entering] = thread(student_func, &s[number_of_students_entering], subject);
			number_of_students_entering++;
			i++;
		}
	}
	
	for(int i = 0; i < n; i++) {
		tid_s[i].join();
	}
	
	cout << "Average waiting time is " << total_waiting_time / n;

	return 0;
}
