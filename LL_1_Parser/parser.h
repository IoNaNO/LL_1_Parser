#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<fstream>
#include<iomanip>
#include<sstream>
using namespace std;
const string infer = "->";

class parser {
private:
	vector<pair<char, string>> gram;
	vector<string> sentences;
	set<char> terms;
	set<char> non_terms;
	map<char, set<char>> firsts;
	map<char, set<char>> follows;
	vector<vector<int>> parsingtable;
	string getproductionbyid(int);
	void clean();
	void findfirst(char);
	void findfollow(char);
	void getFirstFollow();
public:
	parser();
	~parser();
	int read_from_file(string);
	int write_to_file(string);

	void initTermchar();
	void initParsingTable();
};

