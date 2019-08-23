#include "json_parser/naive.cxx"
#include <experimental/filesystem>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;


//[General] : checks if json file is present
//	create lock file
//	read and write sample file to processing file (remove lock file)
//	while processing written file -> write current number to state file
//	when complete delete state file and delete write file and delete lock file
//	return to [General]
//
//

//TODO: create system folder if not already exist

string ENV_HOME = getenv( "HOME" );
string LOCK_FILE = ENV_HOME + "/.afsms/def.lock";
string DEFAULT_COPY_BUFFER = ENV_HOME + "/.afsms/write_buffer.txt";
string DEFAULT_JSON_SMS_FILE = ENV_HOME + "/.afsms/sample_log_file.js";
string DEFAULT_INDEX_FILE = ENV_HOME + "/.afsms/config_index.txt";

namespace fs = std::experimental::filesystem;

void clean_up() {
	std::remove( DEFAULT_JSON_SMS_FILE.c_str() );
	std::remove( DEFAULT_COPY_BUFFER.c_str() );
	std::remove( LOCK_FILE.c_str() );
}


void sleep_process(short int sleep_duration=5) {
	cout <<"[SLEEPING " << sleep_duration << " seconds: " <<flush;
	std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
	cout << "DONE]" << endl;
}

void write_default_json_file(string filename, string data) {
	ofstream write_json_file(filename.c_str());
	write_json_file << data;
	write_json_file.close();
}

bool lock_system() {
	ofstream write_file( LOCK_FILE.c_str() );
	write_file << endl;
	write_file.close();

	ifstream touch_file( LOCK_FILE.c_str());
	bool state = touch_file.good();
	touch_file.close();
	return state;
}


string read_default_json_file(string filename) {
	ifstream read_json_file(filename.c_str());
	if(!read_json_file.good()) {
		sleep_process();
		return "";
	}
	string json_dump(istreambuf_iterator<char>(read_json_file), (istreambuf_iterator<char>()));
	read_json_file.close();
	return json_dump;
}

int get_buffer_index() {
	ifstream readfile( DEFAULT_INDEX_FILE.c_str() );
	int index = 0;
	if(!readfile.good()) return index;

	readfile >> index;
	readfile.close();

	return index;
}

string to_upper(string str) {
	std::transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}

string erase_char(char del, string str) {
	str.erase(std::remove(str.begin(), str.end(), del), str.end());
	return str;
}

void send_sms( auto json_object, int default_buffer_index ) {
	cout<<"[SEND_SMS]: "<<json_object.size()<<endl;
	for(int i=default_buffer_index; i< json_object.size(); ++i) {
		cout <<"[SENDING INDEX: " << i << "]" << endl <<endl;
		json_object[i].stat();
		map<string,string>key_value = json_object[i].get_key_value();
		string number = erase_char( '"', key_value["\"number\""]);
		string message = key_value["\"message\""];
		string service_provider = to_upper( erase_char( '"', key_value["\"service_provider\""] ) );
		string command = "afsms --send --number " + number + " --message " + message + " --group " + service_provider;
		//cout<<"[COMMAND: "<< command << endl;
		system ( command.c_str() );

		ofstream write_index( DEFAULT_INDEX_FILE );
		write_index << i;
		write_index.close();
		sleep_process(20);
	}
	std::remove( DEFAULT_INDEX_FILE.c_str() );
}


int main(int argc, char** argv) {
	bool shown = false;
	cout<<"[ENV_HOME: " << ENV_HOME <<endl;
	cout<<"[LOCK_FILE: " << LOCK_FILE << endl;
	cout<<"[DEFAULT_COPY_BUFFER: " << DEFAULT_COPY_BUFFER << endl;
	cout<<"[DEFAULT_JSON_SMS_FILE: " << DEFAULT_JSON_SMS_FILE << endl;
	cout<<"[DEFAULT_INDEX_FILE: " << DEFAULT_INDEX_FILE << endl;


	while(1) {
		if( string json_string = read_default_json_file( DEFAULT_JSON_SMS_FILE.c_str() ); !json_string.empty() ){
			cout<<"[STATE: FOUND JSON FILE]" << endl;
			shown = false;
			write_default_json_file( DEFAULT_COPY_BUFFER, json_string );
			if( !lock_system() ) {}
			auto json_object = extract_objects( json_string );
			if(json_object.size() < 1) {
				sleep_process(10);
				continue;
			}
			int default_buffer_index = get_buffer_index();
			send_sms( json_object, default_buffer_index );
			clean_up();
		}	
		else {
			if(!shown) cout<<"[STATE: NO JSON FOUND]" << endl;
			shown = true;
			sleep_process(10);
		}
	}
}

