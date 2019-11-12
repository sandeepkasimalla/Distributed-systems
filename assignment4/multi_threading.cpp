#include<iostream>
#include<fstream>
#include<string>
#include<unistd.h>
#include<thread>
#include<vector>
#include<limits>
#include<mutex>
#include<sys/wait.h>
#include<time.h>
using namespace std;

int total = 0 ;
mutex mtx;
struct file_work{
	string file_name;
	int start_index;
	int end_index;
};

void sum_files(vector<file_work> input) {
	int len = input.size();
	int thread_sum = 0;
	for(int i=0; i<len; i++){
		string str = input[i].file_name ;
		int start = input[i].start_index;
		int end = input[i].end_index;
		fstream file(input[i].file_name);
		for(int j=0; j<=input[i].end_index; j++){
			string line;
			getline(file, line);
			if(j >= input[i].start_index){
				thread_sum += stoi(line);
			}
		}
		file.close();
	}
	mtx.lock();
	total += thread_sum;
	mtx.unlock();

}



int main(int argc, char *argv[]){
	if(argc!=6){
		printf("\nUsage: prog <n_atype> <n_btype> <n_numbers_in_file> <n_parent_threads> <n_child_threads>\n\n");
		return 1;
	}

	int n_atype = stoi(argv[1]);
	int n_btype = stoi(argv[2]);
	int n_elem_file = stoi(argv[3]);
	int n_p_threads = stoi(argv[4]);
	int n_c_threads = stoi(argv[5]);
	srand (time(NULL));
	int rand_num ;
	ofstream myfile;
	int actual_sum = 0;

	for(int i=0; i<n_atype; i++){
		string file_name = "a_" + to_string(i) + ".txt" ;
		myfile.open (file_name);
		for(int j=0; j<n_elem_file; j++){
			rand_num = rand() % 100 + 1;
			myfile <<rand_num<<"\n";
			actual_sum += rand_num;
		}
	    myfile.close();
	}

	for(int i=0; i<n_btype; i++){
		string file_name = "b_" + to_string(i) + ".txt" ;
		myfile.open (file_name);
		for(int j=0; j<n_elem_file; j++){
			rand_num = rand() % 100 + 1;
			myfile <<rand_num<<"\n";
			actual_sum += rand_num;
		}
	    myfile.close();
	}

	int pipefds[2];
	pipe(pipefds);

	int var = fork();

	if(var == 0){
		thread child_threads[n_c_threads];
		int curr_file_pos = 0;
		int curr_file = 0;
		int curr_file_elements = n_elem_file;
		int rem = (n_btype*n_elem_file)%n_c_threads;
		int elements_for_thread;
		for (int i = 0; i < n_c_threads; i++) {
			if(rem != 0 && i<rem){
				elements_for_thread = ((n_btype*n_elem_file)/n_c_threads)+1;
			}
			else{
				elements_for_thread = (n_btype*n_elem_file)/n_c_threads;
			}
			vector<file_work> thread_work ;
			while(elements_for_thread){
				struct file_work work;
				if(elements_for_thread >= curr_file_elements){
					work.file_name = "b_" + to_string(curr_file) + ".txt";
					work.start_index = curr_file_pos;
					work.end_index = curr_file_pos + curr_file_elements - 1;
					elements_for_thread -= curr_file_elements;
					curr_file_pos = 0;
					curr_file += 1;
					curr_file_elements = n_elem_file;
				}
				else{
					work.file_name = "b_" + to_string(curr_file) + ".txt";
					work.start_index = curr_file_pos;
					work.end_index = curr_file_pos + elements_for_thread - 1;
					curr_file_pos = curr_file_pos + elements_for_thread;
					elements_for_thread = 0;
					curr_file_elements = n_elem_file - curr_file_pos;
				}
				thread_work.push_back(work);
			}
			child_threads[i] = thread(sum_files, thread_work);
		}

		for (int i = 0; i < n_c_threads; i++){
			child_threads[i].join();
		}
		close(pipefds[0]);
		write(pipefds[1], &total, sizeof(int));
		close(pipefds[1]);

	}
	else{
		thread parent_threads[n_p_threads];
		int curr_file_pos = 0;
		int curr_file = 0;
		int curr_file_elements = n_elem_file;
		int elements_for_thread ;
		int rem = (n_atype*n_elem_file)%n_p_threads;
		for (int i = 0; i < n_p_threads; i++) {
			if(rem != 0 && i<rem){
				elements_for_thread = ((n_atype*n_elem_file)/n_p_threads)+1;
			}
			else{
				elements_for_thread = (n_atype*n_elem_file)/n_p_threads;
			}

			vector<file_work> thread_work ;
			while(elements_for_thread){
				struct file_work work;
				if(elements_for_thread >= curr_file_elements){
					work.file_name = "a_" + to_string(curr_file) + ".txt";
					work.start_index = curr_file_pos;
					work.end_index = curr_file_pos + curr_file_elements - 1;
					elements_for_thread -= curr_file_elements;
					curr_file_pos = 0;
					curr_file += 1;
					curr_file_elements = n_elem_file;
				}
				else{
					work.file_name = "a_" + to_string(curr_file) + ".txt";
					work.start_index = curr_file_pos;
					work.end_index = curr_file_pos + elements_for_thread - 1;
					curr_file_pos = curr_file_pos + elements_for_thread;
					elements_for_thread = 0;
					curr_file_elements = n_elem_file - curr_file_pos;
				}
				thread_work.push_back(work);
			}
			parent_threads[i] = thread(sum_files, thread_work);
		}

		 for (int i = 0; i < n_p_threads; i++){
		 	parent_threads[i].join();
		 }
		 int child_total = 0;
		 close(pipefds[1]);
		 wait(NULL);
		 read(pipefds[0], &child_total, sizeof(int));
		 close(pipefds[0]);
		 int whole_sum = total + child_total;
		 cout <<"Sum calculated by Parent Process threads: "<< total << endl;
		 cout <<"Sum calculated by Child Process threads: "<< child_total << endl;

		 cout<<"Total sum calculated by the Threads: "<<whole_sum<<endl;
		 cout<<"Actual sum: "<<actual_sum<<endl;
		 if(actual_sum == whole_sum)
		 	cout<<"CORRECT : Actual sum matched with the sum calculated by the threads"<<endl;
		 else
		 	cout<<"WRONG : Actual sum din't matched with the sum calculated by the threads"<<endl;


	}
	return 0;
}
