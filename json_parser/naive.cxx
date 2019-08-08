/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
 */



#include <iostream>
#include <map>
#include <vector>
#include "../functions/functions.hpp"


using namespace std;


/*
 * Brain storming session:
 * - global map = parsedJSON
 * - {} = map
 * */

//data types:
//key->value (key, object)
//map<string,string> map<string,object>


class JSON {
	//transform this into a key value object and use a function to traverse through all the data
	map<string, JSON> name_value_store;
	public:
		JSON(){}
		JSON(string json_input):json_input=json_input {

		}

		void extract_map_objects();
		void dev_visualize();
		void add_name_value(string,JSON);
		
		void set_type(string type);
		void set_value(string value);
		void set_value(JSON value);
};

void JSON::set_type(string type){
}

void JSON::set_value(JSON value){
}

void JSON::set_value(string value){
}


void JSON::add_name_value(string name_buffer,JSON value_buffer_object){
	this->name_value_store[name_buffer]=value_buffer_object;
}

void JSON::extract_map_objects(){
	int object_index=-1, object_counter=0;
	JSON* head;
	map<int,JSON*>container;
	map<int,map<int,JSON*>>main_container;
	string name_buffer_map="example_name_buffer";
	JSON value_buffer_map;

	//use this for the example
	value_buffer_map.set_type("string");
	value_buffer_map.set_value("Hello world");
	
	for(auto _char:this->json_input){
		switch(_char){
			case '{':
				++object_index;
				++object_counter;
				head=new JSON;
				break;
			case '}':
				*head.add_name_value(name_buffer_map,value_buffer_map);
				container[object_counter]=head;
				main_container[object_index]=container;
				--object_index;
				break;
				
		}
	}
}
void JSON::dev_visualize(){}

int main(int argc, char** argv) {
	string sample_json="{\"nam:{e\" :\"sherlock wisdom\",\"name2\":\"sherlock holmes\",new_object:{\"new_name\":\"new_sherlock_wisdom\"}}";
	string sample_json_1="{\"name\":\"sherlock wisdom\",new_object:{}}";

	JSON json_object(sample_json_1);
	json_object.extract_map_objects();
	json_object.dev_visualize();
	return 0;
}
