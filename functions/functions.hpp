#ifndef FUNCTIONS_H_INCLUDED_
#define FUNCTIONS_H_INCLUDED_
#include <iostream>
#include <vector>
#include <fstream>
#include <map>



using namespace std;


string strVector_to_string(vector<string> _vector) {
	//Add a spacing delimeter to make string more edible
	string return_value="";

	for(auto _string : _vector) {
		return_value+=_string;
	}

	return return_value;
}


vector<string> str_split(string _string, char del = ' ') {
	vector<string> return_value;
	string temp_string = "";
	for(auto _char : _string) {
		if(_char==del) {
			return_value.push_back(temp_string);
			temp_string="";
		}
		else {
			temp_string+=_char;
		}
	}
	return_value.push_back(temp_string);

	return return_value;
}

#endif
