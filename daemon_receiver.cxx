//{"user_id":"fd8e4752-1328-45f8-acd9-72091de08926","region_id":"","community_id":"","name":"Rr","age":"55","gender":"female","date_of_test_request":"2019-08-23","address":"Vv","telephone1":"123456789","telephone2":"","art_code":"unknown","reason_for_test":"presumptive_tb","ward_bed_number":"out patient","patient_category":["plh"],"specimen_type":["lymph",{"other":""}],"tb_treatment_history":{"tb_treatment_history":"new","tb_his_other":""},"symptoms":["night_sweats"],"requester":{"name":"","email":"","phonenumber":"","other":"","date":""},"mhealth_data_class":"new_patient","uuid":"eddc4f48-b6a4-4a7a-b325-dc6d9e6cb3ee","mhealth_stored_time":"2019-08-23T11:46:55.347Z","id":22}


#include <iostream>
#include "json_parser/naive.cxx"
#include "functions/functions.hpp"
#include <map>
#include <chrono>
#include <thread>
using namespace std;

map<string, string> list_of_servers {
	{"\"icftbrlbamenda\"", "node node/tbrlbamenda.js"}
};

string pipe_terminal(string command) {
	string data;
	FILE * stream;
	const int max_buffer = 1024;
	char buffer[max_buffer];
	command.append(" 2>&1");

	stream = popen(command.c_str(), "r");
	if (stream) {
		while (!feof(stream)) if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
		pclose(stream);
	}
	return data;
}


void sleep_process(short int sleep_duration=5) {
	cout <<"[SLEEPING " << sleep_duration << " seconds: " <<flush;
	std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
	cout << "DONE]" << endl;
}


string extract_target_id( string message ) {

	auto json_message = extract_objects( message );
	
	if(json_message.empty()) {
		cout << "[STATUS]: no json message found..." << endl;
		return "";
	}
	map<string,string> json_key_value = json_message[0].get_key_value();
	for(auto i : json_key_value) cout << "[JSON MAP]: " << i.first << endl;
	string target_id = json_key_value["\"target_id\""];
	cout << "\t[TARGET ID]: " << target_id << endl;


	return target_id;
}

void send_to_script(string target_id, string message) {

	if(list_of_servers.find( target_id ) != list_of_servers.end() ) { //or something similar
		cout <<"[STATUS]: found script for target id!" << endl;
		string path_to_script = list_of_servers[ target_id ];
		string command = path_to_script + " --sms_data " + message;
		//cout << "\t[TERMINAL COMMAND]: " << command << endl;
		
		//naive version
		system( command.c_str() );
		cout << "[STATUS]: sent message to script!" << endl;
	}
	else {
		cout << "[STATUS]: script not found for: " << message << endl;
	}

}


int main(int argc, char** argv) {
	//check for incoming sms messages
	//check target id of messages and send to right receipients
	

	//daemon looking for sms
	//
	cout<<"[ARGC]: " << argc << endl;
	if(argc > 1 and (string) argv[1] == "--test") {
		//string message = (string) argv[1];
		string message = "'{\"user_id\":\"fd8e4752-1328-45f8-acd9-72091de08926\",\"region_id\":\"\",\"community_id\":\"\",\"name\":\"Rr\",\"age\":\"55\",\"gender\":\"female\",\"date_of_test_request\":\"2019-08-23\",\"address\":\"Vv\",\"telephone1\":\"123456789\",\"telephone2\":\"\",\"art_code\":\"unknown\",\"reason_for_test\":\"presumptive_tb\",\"ward_bed_number\":\"out patient\",\"patient_category\":[\"plh\"],\"specimen_type\":[\"lymph\",{\"other\":\"\"}],\"tb_treatment_history\":{\"tb_treatment_history\":\"new\",\"tb_his_other\":\"\"},\"symptoms\":[\"night_sweats\"],\"requester\":{\"name\":\"\",\"email\":\"\",\"phonenumber\":\"\",\"other\":\"\",\"date\":\"\"},\"mhealth_data_class\":\"new_patient\",\"uuid\":\"eddc4f48-b6a4-4a7a-b325-dc6d9e6cb3ee\",\"mhealth_stored_time\":\"2019-08-23T11:46:55.347Z\",\"id\":22, \"target_id\":\"icftbrlbamenda\"}'";
		cout << "[TEST|MESSAGE]: " << message << endl;
		string target_id = extract_target_id( message );
		if(target_id.empty()) {
			cout<<"[TEST]: no target_id found!" << endl;
			return 1;
		}
		send_to_script( target_id , message);
	}
	else {
		while( 1 ) {
			string modem_indexes = pipe_terminal("scripts/modem_information_extraction.sh list");
			auto modems = str_split( modem_indexes, '\n' );
			for(auto index : modems ) {
				if(index.empty()) continue;
				cout << "[MODEM]: " << index << "\n----------------" << endl;
				string command = "./scripts/modem_information_extraction.sh sms received " + index; 
				string sms_indexes = pipe_terminal( command.c_str() );
				if(!sms_indexes.empty()) cout << "[STATE]: found sms indexes...\n" << sms_indexes << endl;
				auto message_indexes = str_split( sms_indexes, '\n' );
				cout << "[STATUS]: found " << message_indexes.size() << " messages" << endl;
				for(auto message_index : message_indexes) {
					if(message_index.empty()) continue;
					string command = "./scripts/modem_information_extraction.sh sms read_sms " + message_index + " " + index;
					cout << "\t[COMMAND]: " << command << endl;
					string meta_sms_information = pipe_terminal( command.c_str() );
					cout << "\t[META_SMS_INFORMATION]: " << meta_sms_information << endl;
					auto sms_container = str_split( meta_sms_information , '\n', true);
					if(sms_container.size() != 3) continue; //1 = number, 2 = message, 3 = timestamp

					string target_id = extract_target_id( sms_container[1] );
					if(target_id.empty()) continue;
					send_to_script( target_id , sms_container[1] );
					sleep_process(20);
				}
				cout << "[MODEM]: done....\n------" << endl;
			}

		}
	}
	return 0;
}
