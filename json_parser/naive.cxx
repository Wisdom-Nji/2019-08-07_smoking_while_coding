/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
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
	
	vector<Json>name_object_buffer;

	enum write_state{NAME,VALUE};
	write_state WRITE_STATE=NAME;
	public:
		Json(){}
		Json(string standard_name):standard_name(standard_name){}
		void write_head_switch(), new_data(), write_data(char), add_child(Json), stat(int);
		string get_name_buffer() { return this->name_buffer; }
		string get_standard_name() { return this->standard_name; }
};


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
	this->name_value_buffer.insert(make_pair(this->name_buffer,this->value_buffer));
	this->name_buffer="";
	this->value_buffer="";
	this->WRITE_STATE=NAME;
}

void Json::write_data(char _char){
	if(this->WRITE_STATE==NAME) this->name_buffer+=(char)_char;
	else if(this->WRITE_STATE==VALUE) this->value_buffer+=(char)_char;
}

void Json::add_child(Json json_object){
	this->name_object_buffer.push_back(json_object);
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
		else cout<<tabs()<<endl<<name_value.first<<" ---::"<<endl;
	}
	
	if(!this->name_object_buffer.empty()) {
		cout<<tabs()<<this->standard_name<<"--->[name_object_buffer_size: "<<this->name_object_buffer.size()<<"]"<<endl;
		short int tab_index=1;
		for(auto object:this->name_object_buffer){
			object.stat(++tab_index);
		}
	}
	//else {}
	cout << endl;
}


auto extract_objects(string sample_string, int& starter_index=0) {
	vector<Json>previous_last_one_stack;
	
	if(starter_index!=0) {
		Json json;
		previous_last_one_stack.push_back(json);
	}
	vector<Json>previous_last_one_stack_array;
	bool ignore_special_chars=false;
	for(int i=0;i<sample_string.size();++i){
		char _char=sample_string[i];
		cout<<"["<<_char<<": "<<i<<" - ";
		switch(_char){
			case '[':{
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
					 Json json;
					 previous_last_one_stack_array.push_back(json);
					 begin_array=true;
					 break;
				 }
			case ']':{
					 // ["string", "string", 10]
					 // [{"name":"sherlock", age:10}]
					 // [{"name":"sherlock", age:10},{"name":"sherlock", age:10}]
					 // [{"name":"sherlock", age:10},["string","string","string"]}]
					 if(ignore_special_chars) {
						 previous_last_one_stack.back().write_data(_char);
						 break;
					 }
					 previous_last_one_stack_array.pop_back();
					 begin_array=false;
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
				 previous_last_one_stack.back().write_data('\\');
			default:
				 previous_last_one_stack.back().write_data(_char);
				 break;
		}
		cout << " done]" <<endl;

	}
	return previous_last_one_stack;
}



int main(int argc, char** argv){
	//string sample_string="{name:sherlock,object:{new_object:{},new_object_2:{}}}";
	string sample_string="{\"nam:{e\" :\"sherlock wisdom\",\"name2\":\"sherlock holmes\",new_object:{\"new_name\":\"new_sherlock_wisdom\"},\"zinal_help\":\"yes I shall help\"}";

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
