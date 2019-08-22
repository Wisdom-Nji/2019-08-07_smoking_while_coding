#include "json_parser/naive.cxx"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;


void clean_up() {
	system("rm ../TmpNodeRouting/sample_log_file.js");
	system("rm ../TmpNodeRouting/afsms.lock");
}


int main(int argc, char** argv) {
	while(1){
		system("touch ../TmpNodeRouting/afsms.lock");
		ifstream read_json_file("../TmpNodeRouting/sample_log_file.js");

		string json_dump(istreambuf_iterator<char>(read_json_file), (istreambuf_iterator<char>()));

		//cout << json_dump << endl;
		
		auto objects = extract_objects(json_dump);

		for(auto object:objects){
			object.stat();
		}
		clean_up();
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}
