/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
 */



#include <iostream>
#include <map>
#include <vector>


using namespace std;


int main(int argc, char** argv) {
	string sample_json="{\"name\" :\"sherlock wisdom\"}";
	int map_creation_index=-1;
	vector<map<string,string>>mapped;
	bool do_key=true,do_value=false,escape=false,allow_white_space=false;
	string key_buffer="",value_buffer="";

	//fuck, time to mess around with pointers
	string* write_head = &key_buffer;
	//discover all map objects
	
	for(int i=0;i<sample_json.size();++i){
		if(sample_json[i]=='"'){
			escape=true;
			allow_white_space=!allow_white_space;
		}

		//else if(sample_json[i]==' ' and !allow_white_space)continue;

		if(allow_white_space or escape) {
			if(escape)*write_head+='\\';
			*write_head+=sample_json[i];
		}

		else 
		if(sample_json[i]=='{') {
			if(i==0) { /* global map begins here */ }
			++map_creation_index;
		}

		else
		if(sample_json[i]==':') {
			if(do_key){
				do_value=true;
				do_key=false;
				write_head=&value_buffer;
			}

		}

		//with , do_key becomes true whenever a comma is reached and write_head=key_buffer
		else
		if(sample_json[i]=='}'){

			//insert into map
			//get map at index position
			map<string,string>temp_map={{key_buffer,value_buffer}};
			mapped.push_back(temp_map);
			--map_creation_index;
		}

		escape=false;

	}

	cout<<"Number of objects: "<<mapped.size()<<endl;

	for(auto key_value_map:mapped){
		map<string,string>key_value=key_value_map;
		for(auto _map:key_value) cout<<_map.first<<"="<<_map.second<<endl;

	}

	return 0;
}
