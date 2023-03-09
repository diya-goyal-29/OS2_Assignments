//============================================================================
// Name        : Assgn3_src_cs20btech11014.cpp
// Author      : Diya Goyal
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

using namespace std;

class process_info {
public:
	int pid;
	int execution_time;
	int period;
	int k;
	int waiting_time = 0;
	int real_time_execution = 0;
	int counter = 0;
	int missed = 0;
};

int calculate_priority(vector<process_info>& real_time_exec,vector<process_info>& process, int *max_running_time) {
	int priority = -1, least_period = *max_running_time;
	for (unsigned int i = 0; i < real_time_exec.size(); i++) {
		if (real_time_exec[i].execution_time != 0) {
			if(least_period > process[i].period) {
				priority = i;
				least_period = process[i].period;
			}
		}
	}

	return priority;

}

void scheduler(vector<process_info>& real_time_exec, vector<process_info>& process, int *max_running_time, vector<int>& queue) {
	int time, priority;

	for(time = 0; time < *max_running_time; time++) {

		priority = calculate_priority(real_time_exec, process, max_running_time);

		queue.push_back(priority);

		real_time_exec[priority].execution_time--;
		real_time_exec[priority].real_time_execution++;
		for (unsigned int i = 0; i < real_time_exec.size(); i++) {
			if (i != (unsigned int)priority) {
				if (real_time_exec[i].execution_time != 0) {
					if (real_time_exec[i].period != 0 && real_time_exec[i].k > 0) {
						process[i].waiting_time++;

					}
				}
			}
		}

		for(unsigned int i = 0; i < real_time_exec.size(); i++) {
			real_time_exec[i].period--;
			if(real_time_exec[i].period == 0 && real_time_exec[i].k > 1) {
				if(real_time_exec[i].real_time_execution != process[i].execution_time) {
					process[i].missed++;

				}
				real_time_exec[i].execution_time = process[i].execution_time;
				real_time_exec[i].period = process[i].period;
				real_time_exec[i].real_time_execution = 0;
				real_time_exec[i].k--;
			}
		}
	}

}

int main() {
	ifstream input_file;
	ofstream stat_file, log_file;

	input_file.open("inp_params.txt");
	stat_file.open("RM_stat.txt");
	log_file.open("RM_log.txt");

	int num_proc;
	input_file >> num_proc;

	vector<process_info> process;
	vector<process_info> real_time_exec;
	vector<int> queue;

	process_info temp;
	for(int i = 0; i < num_proc; i++) {
		input_file >> temp.pid >> temp.execution_time >> temp.period >> temp.k;
		process.push_back(temp);
		real_time_exec.push_back(temp);
		log_file << "Process P" << process[i].pid << ": processing time = " << process[i].execution_time <<"; deadline : " << process[i].period << "; period : " << process[i].period << " joined the system at time 0 " << endl;
	}

	int max_running_time = 0;
	for (int i = 0; i < num_proc; i++) {
		if (max_running_time < (process[i].k * process[i].period)) {
			max_running_time = process[i].k * process[i].period;
		}
	}

	scheduler(real_time_exec, process, &max_running_time, queue);

	int process_id;
	process_id = queue[0];
	for(int i = 0; i < max_running_time; i++) {

		if(process_id == queue[i]) {
			process[process_id].counter++;
		}
		else if(process_id == -1) {
			log_file << "CPU is IDLE till " << i << endl;
			process[process_id].counter = 0;
			process_id = queue[i];
			process[process_id].counter++;
		}
		else if(process[process_id].counter == process[process_id].execution_time) {
			log_file << "Process P" << process_id + 1 <<" starts execution at time " << i - process[process_id].counter << "." << endl;
			log_file << "Process P" << process_id + 1 <<" ends execution at time " << i << "." << endl;
			process[process_id].counter = 0;
			process_id = queue[i];
			process[process_id].counter++;
		}
		else if(process[process_id].counter != process[process_id].execution_time) {
			log_file << "Process P" << process_id + 1 <<" starts execution at time " << i - process[process_id].counter << "." << endl;
			log_file << "Process P" << process_id + 1 << " is preempted by Process P" << queue[i] + 1 << " at time " << i << ". Remaining processing time: " << process[process_id].execution_time - process[process_id].counter << "." << endl;
			process[process_id].counter = 0;
			process_id = queue[i];
			process[process_id].counter++;
		}

	}
	float average_waiting_time;
	int total_waiting_time = 0;
	int total_misses = 0, total_num_procs = 0;
	for (int i = 0; i < num_proc; i++) {
		stat_file << "Number of times P" << process[i].pid << " came into the system is : " << process[i].k << endl;
		stat_file << "Number of times P"  << process[i].pid << " successfully completed : " << process[i].k - process[i].missed << endl;
		stat_file << "Number of misses for P" << process[i].pid << " is : " << process[i].missed << endl;
		average_waiting_time = (float)process[i].waiting_time / process[i].k;
		total_misses += process[i].missed;
		total_num_procs += process[i].k;
		total_waiting_time += process[i].waiting_time;
		stat_file << "Average waiting time of P" << process[i].pid << " is : " << average_waiting_time  << endl;
	}

	stat_file << endl << "Total number of misses : " << total_misses << endl;
	stat_file << "Total average waiting time : " << (float)total_waiting_time / total_num_procs << endl;
	stat_file << "\n" << endl;

	log_file.close();
	stat_file.close();
	input_file.close();

	cout << "RMS DONE\n" << endl;

	return 0;
}
