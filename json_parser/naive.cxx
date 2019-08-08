/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
 */



#include <iostream>
#include <map>
#include <vector>


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
	bool state_key,state_value;

	void set_object_write_head(map<string,JSON>&);
	void write_object();

	map<string,JSON>* head;

	map<int,map<string,JSON>>main_container;
	int object_index=-1;

	string json_string;
	string name_buffer;
	JSON* value_buffer_pnt;


	void change_state();
	void write_to_data_head();
	public:
		JSON(){
			this->name_buffer="";
		}
		JSON(string json_string):json_string(json_string){
			this->name_buffer="";
			this->value_buffer_pnt=new JSON(); 
		};

		void parse();
		void debug_visualize();


};

void JSON::set_object_write_head(map<string,JSON>& object_index){
	this->head=&object_index;
}

void JSON::write_object() {
	string name=this->name_buffer;
	JSON value=*this->value_buffer_pnt;
	map<string,JSON>tmp_head_with_val=*this->head;
	tmp_head_with_val.insert(make_pair(name,value));
	*this->head=tmp_head_with_val;
	this->name_buffer="";
	this->value_buffer_pnt=new JSON();
}

void JSON::parse() {
	map<string,JSON>object;
	for(auto _char:this->json_string){
		switch(_char){
			case '{':
				++this->object_index;
				cout<<"object_index="<<this->object_index<<endl;
				this->main_container[this->object_index]=object;
				this->set_object_write_head(this->main_container[this->object_index]);
				break;
			case '}':
				cout<<"object_index="<<this->object_index<<endl;
				this->set_object_write_head(this->main_container[this->object_index]);
				this->write_object();
				--this->object_index;
				break;
			case ':':
				cout<<"Working in index="<<this->object_index<<endl;
				//fit for value buffer
				this->change_state();
				break;
			case ',':
				cout<<"Working in index="<<this->object_index<<endl;
				//add more to current map
				this->write_object();
				break;
			default:
				write_to_data_head(_char);
				break;

		}
	}
}

void JSON::write_to_data_head(char _char){
	if(this->state_name){
		this->name_buffer+=_char;
	}
	else if(this->state_value) {
		this->
	}
}

void JSON::change_state() {
	this->state_key=!state_key;
	this->state_value=!state_value;
}

void JSON::debug_visualize() {
	cout<<"main_container_size(): "<<this->main_container.size()<<endl;
	for(auto map_iterator:this->main_container){
		map<string,JSON>_map=map_iterator.second;
		for(auto __map:_map) {
			cout<<__map.first<<"="<<endl;
		}
	}
}


int main(int argc, char** argv) {
	string sample_json="{\"nam:{e\" :\"sherlock wisdom\",\"name2\":\"sherlock holmes\",new_object:{\"new_name\":\"new_sherlock_wisdom\"}}";
	string sample_json_1="{\"name\":\"sherlock wisdom\",new_object:{}}";
	JSON json(sample_json_1);
	json.parse();
	json.debug_visualize();


	/*int map_creation_index=-1;
	vector<map<string,string>>mapped;
	map<int,map<string,string>>parsedJsonObject;
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
		if(sample_json[i]==','){
			if(do_value) {
				do_value=false;
				do_key=true;

				write_head=&key_buffer;
				map<string,string>temp_map={{key_buffer,value_buffer}};
				mapped.push_back(temp_map);
				key_buffer="";
				value_buffer="";
			}
		}

		else 
		if(sample_json[i]=='{') {
			++map_creation_index;
			parsedJsonObject.insert(make_pair(map_creation_index, (map<string,string>){}));
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

	}*/

	return 0;
}
