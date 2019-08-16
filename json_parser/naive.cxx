/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
 *Might be a daddy today hahaha
 */
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <fstream>
using namespace std;


class Json{
	string* write_head;
	string name_buffer="",value_buffer="", standard_name;

	map<string,string>name_value_buffer;
	
	vector<Json>name_object_buffer, name_array_buffer;
	vector<string>array_strings;

	enum write_state{NAME,VALUE};
	write_state WRITE_STATE=NAME;

	bool array_value=false;
	public:
		Json(){}
		Json(string standard_name):standard_name(standard_name){}
		void write_head_switch(), new_data(), write_data(char), add_child(Json), stat(int), set_is_array(bool);
		string get_name_buffer() { return this->name_buffer; }
		string get_standard_name() { return this->standard_name; }

		bool is_array() { return this->array_value; }
};


void Json::set_is_array(bool is_array) {
	this->array_value=is_array;
}


void Json::write_head_switch(){
	if(this->WRITE_STATE==NAME) this->WRITE_STATE=VALUE;
	else if(this->WRITE_STATE==VALUE) this->WRITE_STATE=NAME;
	//cout<<"WRITE_STATE_AFTER_SWITCH: " <<this->WRITE_STATE<<endl;
}

void Json::new_data() {
	if(this->name_buffer.empty()) {
		//cout<<"WRITE_STATE="<<this->WRITE_STATE<<endl;
		return;
	}
	//cout<<"--making new data: "<<this->name_buffer<<"|"<<this->value_buffer<<endl;
	if(!this->array_value) this->name_value_buffer.insert(make_pair(this->name_buffer,this->value_buffer));
	else this->array_strings.push_back(this->name_buffer);
	this->name_buffer="";
	this->value_buffer="";
	this->WRITE_STATE=NAME;
}

void Json::write_data(char _char){
	if(this->WRITE_STATE==NAME) this->name_buffer+=(char)_char;
	else if(this->WRITE_STATE==VALUE) this->value_buffer+=(char)_char;
}

void Json::add_child(Json json_object){
	if(json_object.is_array()) this->name_array_buffer.push_back(json_object);
	else this->name_object_buffer.push_back(json_object);
}


void Json::stat(int tab_index=0){
	auto tabs=[=](){
		string ret_value="";
		for(int i=0;i<tab_index;++i) ret_value.push_back('\t');
		return ret_value;
	};
	cout<<tabs()<<this->standard_name<<"--->[name_value_buffer_size: "<<this->name_value_buffer.size()<<"]"<<endl;
	for(auto name_value:this->name_value_buffer){
		if(!name_value.second.empty()) cout<<tabs()<<name_value.first<<"="<<name_value.second<<endl;
		else cout<<tabs()<<name_value.first<<" ---::"<<endl;
	}
	
	if(!this->name_object_buffer.empty()) {
		cout<<tabs()<<this->standard_name<<"--->[name_object_buffer_size: "<<this->name_object_buffer.size()<<"]"<<endl;
		for(auto object:this->name_object_buffer){
			object.stat(++tab_index);
		}
	}

	cout<<tabs()<<this->standard_name<<"--->[array: "<<this->name_array_buffer.size()<<"]"<<endl;
	for(auto array:this->name_array_buffer)
		array.stat(++tab_index);
	for(auto _string:this->array_strings)
		cout<<tabs()<<_string<<endl;
	cout << endl;
}


auto extract_objects(string sample_string) {
	//The solution to the problem lies in recursive loop functions ######### fucking use them
	//Thought: Return should send back previous_last_one_stack, if it's got an object it shows and object and if it's got an array, it shows an array
	vector<Json>previous_last_one_stack; //temporarily stores objects
	vector<Json>previous_last_one_stack_array; //temporarily stores arrays
	bool ignore_special_chars=false;
	for(int i=0;i<sample_string.size();++i){
		char _char=sample_string[i];
		//cout<<"["<<_char<<": "<<i<<" - ";
		switch(_char){
			case '[':{
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
					 cout<<"Name buffer: "<<previous_last_one_stack.back().get_name_buffer()<<endl;
					 Json json(previous_last_one_stack.back().get_name_buffer());
					 json.set_is_array(true);
					 previous_last_one_stack.push_back(json);
					 //thought: when new object, begin new object, store the array value.
					 //write everything to the object when you beg
					 break;
				 }
			case ']':{

					 //TODO: #ElonSays "to nuke mars"... traverse through tree structure using a predictive model as a feedforward function loop
					 // ["string", "string", 10]
					 // [{"name":"sherlock", age:10}]
					 // [{"name":"sherlock", age:10},{"name":"sherlock", age:10}]
					 // [{"name":"sherlock", age:10},["string","string","string"]}]
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
				 	 previous_last_one_stack.back().new_data();
				 	 previous_last_one_stack.at(previous_last_one_stack.size() -2).add_child(previous_last_one_stack.back());
					 previous_last_one_stack.pop_back();
					 break;
				 }
			case '{':{
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
					 if(!previous_last_one_stack.empty()) {
						 //cout << "name_buffer: " << previous_last_one_stack.back().get_name_buffer();
						 Json json(previous_last_one_stack.back().get_name_buffer());
						 //cout << json.get_standard_name() << " -- ";
						 previous_last_one_stack.back().new_data();
						 previous_last_one_stack.push_back(json);
						 //cout << previous_last_one_stack.back().get_standard_name() << " -- ";
					 }
					 else {
						 Json json;
						 previous_last_one_stack.push_back(json);
					 }
					 //previous_last_one_stack.push_back(i);
					 break;
				 }
			case '}':{
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
					 if(previous_last_one_stack.size() > 1){
						 /*cout<<"[EOO: parent available... |";
						 cout << previous_last_one_stack.back().get_standard_name() << " -- ";
						 cout << previous_last_one_stack.back().get_name_buffer() << " -- ";*/
						 previous_last_one_stack.back().new_data();
						 previous_last_one_stack.at(previous_last_one_stack.size() -2).add_child(previous_last_one_stack.back());
						 previous_last_one_stack.pop_back();
					 }
					 else {
						 previous_last_one_stack.at(previous_last_one_stack.size() -1).new_data();
					 }

					break;
				 }
			case ':':
				 if(ignore_special_chars) {
					 previous_last_one_stack.back().write_data(_char);
					 break;
				 }
				 previous_last_one_stack.back().write_head_switch();
				 break;
			case ',':
				 if(ignore_special_chars) {
					 previous_last_one_stack.back().write_data(_char);
					 break;
				 }
				 previous_last_one_stack.back().new_data();
				 break;
			case '"':
				 ignore_special_chars=!ignore_special_chars;
				 //previous_last_one_stack.back().write_data('\\');
			default:
				 previous_last_one_stack.back().write_data(_char);
				 break;
		}
		//cout << " done]" <<endl;

	}
	return previous_last_one_stack;
}



int main(int argc, char** argv){
	//string sample_string="{name:sherlock,object:{new_object:{},new_object_2:{}}}";
	string sample_string="{\"array_value\":[\"sample_array_string\", 10],\"nam:{e\" :\"sherlock wisdom\",\"name2\":\"sherlock holmes\",new_object:{\"new_name\":\"new_sherlock_wisdom\"},\"zinal_help\":\"yes I shall help\"}";

	auto objects = extract_objects(sample_string);


	for(auto object:objects){
		object.stat();
	}

	/*
	ifstream read_json_file("sample_json_file.js");

	string json_dump(istreambuf_iterator<char>(read_json_file), (istreambuf_iterator<char>()));

	//cout << json_dump << endl;
	
	objects = extract_objects(json_dump);

	for(auto object:objects){
		object.stat();
	}*/

	return 0;
}
