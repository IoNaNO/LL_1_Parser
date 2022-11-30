#include"parser.h"

parser::parser() {

}

parser::~parser() {

}

void parser::clean() {
	gram.clear();
	sentences.clear();
	terms.clear();
	non_terms.clear();
	firsts.clear();
	follows.clear();
	parsingtable.clear();
}

int parser::read_from_file(string filename) {
	fstream infile;
	infile.open(filename, ios::in);
	if (infile.is_open() == false) {
		//cerr << "No such file!" << endl;
		//exit(-1);
		return -1;
	}

	// read grammar file line by line
	clean();
	int n=0;
	infile >> n;
	if (!n) {
		return -2;
	}
	while (n--) {
		string line;
		infile >> line;
		char lhs;
		auto it = line.begin();
		lhs = *it++;
		if (!isupper(lhs)) {
			// wrong format
			return -2;
		}
		string op(it, it + 2);
		if (op != infer) {
			return -2;
		}
		it += 2;
		auto i = it;
		for (; i < line.end(); i++) {
			if (!isalpha(*i)) {
				if (*i == '|') {
					string sub(it, i);
					gram.push_back(make_pair(lhs,sub));
					it =i+1;
				}
			}
		}
		string sub(it, i);
		gram.push_back(make_pair(lhs,sub));
	}
	cout << "Grammer parsed successfully! The result is: " << endl;
	int cnt = 1;
	for (auto& m : gram) {
		cout << cnt++ << ". " << m.first << infer << m.second << endl;
	}
	n = 0;
	infile >> n;
	if (!n) {
		return -2;
	}
	sentences.resize(n);
	for (int i = 0; i < n; i++) {
		infile >> sentences[i];
	}
	infile.close();
	return 0;
}

int parser::write_to_file(string filename = "output.txt") {
	fstream outfile;
	outfile.open(filename, ios::out);
	if (!outfile.is_open()) {
		return -1;
	}
	// write grammar
	outfile << "The grammar is : " << endl;
	int cnt = 1;
	for (auto& m : gram) {
		outfile << cnt++ << ". " << m.first << infer << m.second << endl;
	}
	// write firsts
	outfile << "The first set : " << endl;
	for (auto& m : firsts) {
		outfile << m.first << ": ";
		for (auto& k : m.second) {
			outfile << k << ' ';
		}
		outfile << endl;
	}
	// write follows
	outfile << "The follow set : " << endl;
	for (auto& m : follows) {
		outfile << m.first << ": ";
		for (auto& k : m.second) {
			outfile << k << ' ';
		}
		outfile << endl;
	}
	// write parsing table
	outfile << "The parsing table : " << endl;
	outfile <<setiosflags(ios::left) << setw(10) << ' ';
	for (auto& m : terms) {
		outfile<< setiosflags(ios::left) << setw(10)<<m;
	}
	outfile << endl;
	for (auto rit = non_terms.begin(); rit != non_terms.end(); rit++) {
		outfile << setiosflags(ios::left) << setw(10)<< * rit;
		for (int col = 0; col < terms.size(); col++) {
			int row = distance(non_terms.begin(), rit);
			int prod_num = parsingtable[row][col];
			if ( prod_num == -1) {
				outfile << setiosflags(ios::left) << setw(10)<< "-";
			}
			else {
				stringstream ss;
				ss << prod_num + 1 << "." << getproductionbyid(prod_num);
				string s;
				ss >> s;
				outfile<<setiosflags(ios::left) << setw(10) <<s;
			}
		}
		outfile << endl;
	}
	// process the sentences
	for (auto str : sentences) {
		cout << "start process " << str << endl;
		outfile << str << ", the result is: " << endl;
		vector<char> stack;
		str += '#';
		stack.push_back('#');
		stack.push_back(gram[0].first);
		char top=str[0];
		auto it = str.begin();
		bool issuccuss = true;
		while (top!='#') {
			top = stack.back();
			if (terms.find(top) != terms.end() || top == 'e') {
				if (top == *it) {
					stack.pop_back();
					it++;
				}
				else {
					cerr << "process: Do not match!" << endl;
					outfile << "process failed!" << endl;
					//outfile.close();
					//return -3;
					issuccuss = false;
					break;
				}
			}
			else if(non_terms.find(top)!=non_terms.end()&&terms.find(*it)!=terms.end()) {
				int row = distance(non_terms.begin(), non_terms.find(top));
				int col = distance(terms.begin(),terms.find(*it));
				int prod_num = parsingtable[row][col];
				if (prod_num == -1) {
					cerr << "process: No production found in parse table!" << endl;
					outfile << "process failed!" << endl;
					//outfile.close();
					//return -3;
					issuccuss = false;
					break;
				}
				outfile << getproductionbyid(prod_num) << endl;
				stack.pop_back();
				string rhs = gram[prod_num].second;
				if (rhs == "e") {
					continue;
				}
				for (auto r = rhs.rbegin(); r < rhs.rend(); r++) {
					stack.push_back(*r);
				}
			}
			else {
				cerr << "process: Invalid input!" << endl;
				outfile << "process failed!" << endl;
				//outfile.close();
				//return -3;
				issuccuss = false;
				break;
			}
		}
		if(issuccuss)
			cout << "process successfully!" << endl;
	}
	outfile.close();
	return 0;
}

void parser::initTermchar() {
	// get all non terminals
	for (auto& m : gram) {
		non_terms.insert(m.first);
	}

	// get all terminals
	for (auto& m : gram) {
		for (auto& c : m.second) {
			if (!isupper(c)) {
				terms.insert(c);
			}
		}
	}
	// remove epsilon and add #(end character)
	terms.erase('e');
	terms.insert('#');

	getFirstFollow();
}

void parser::getFirstFollow() {
	// get first set, start with non terminal
	for (auto& m : non_terms) {
		if (firsts[m].empty()) {
			findfirst(m);
		}
	}
	cout << "Find first successfully! The result is: " << endl;
	for (auto& m : firsts) {
		cout << m.first << ": ";
		for (auto& k : m.second) {
			cout << k << ' ';
		}
		cout << endl;
	}
	// init with #
	follows[gram.begin()->first].insert('#');
	findfollow(gram.begin()->first);
	// get follow set, start with non terminal
	for (auto& m : non_terms) {
		if (follows[m].empty()) {
			findfollow(m);
		}
	}
	cout << "Find Follow successfully! The result is: " << endl;
	for (auto& m : follows) {
		cout << m.first << ": ";
		for (auto& k : m.second) {
			cout << k << ' ';
		}
		cout << endl;
	}
}

void parser::findfirst(char non_term) {
	for (auto& m : gram) {
		if (m.first != non_term) {
			continue;
		}
		string rhs = m.second;
		// loop till a non terminal or epsilon found
		for (auto it = rhs.begin(); it < rhs.end(); it++) {

			if (non_terms.find(*it) == non_terms.end()) {
				firsts[non_term].insert(*it);
				break;
			}
			else {
				// if find a non terminal , use recursion to find first
				if (firsts[*it].empty()) {
					findfirst(*it);
				}
				set<char> copy(firsts[*it]);
				if (copy.find('e') != copy.end()) {
					// if not the last, remove epsilon
					if (it + 1 != rhs.end()) {
						copy.erase('e');
					}
					firsts[non_term].insert(copy.begin(), copy.end());
				}
				else {
					firsts[non_term].insert(copy.begin(), copy.end());
					break;
				}
			}
		}
	}
}

void parser::findfollow(char non_term) {
	for (auto& m : gram) {
		string k = m.second;
		bool isfinsh = true;
		auto it = k.begin();
		// find non_term
		for (; it < k.end(); it++) {
			if (*it == non_term) {
				isfinsh = false;
				break;
			}
		}
		if (it != k.end()) {
			it++;
		}
		for (; it < k.end() && !isfinsh; it++) {
			// if terminal , append to follow
			if (non_terms.find(*it) == non_terms.end()) {
				follows[non_term].insert(*it);
				isfinsh = true;
				break;
			}
			set<char> copy(firsts[*it]);
			// if find epsilon, erase and continue
			if (copy.find('e') != copy.end()) {
				copy.erase('e');
				follows[non_term].insert(copy.begin(), copy.end());
			}
			else {
				follows[non_term].insert(copy.begin(), copy.end());
				isfinsh = true;
				break;
			}
		}
		// in the end of production, use recursion to follow
		if (it == k.end() && !isfinsh) {
			if (follows[m.first].empty()) {
				findfollow(m.first);
			}
			follows[non_term].insert(follows[m.first].begin(), follows[m.first].end());
		}
	}
}

void parser::initParsingTable()
{
	parsingtable.clear();
	parsingtable.resize(non_terms.size(), vector<int>(terms.size(), -1));
	for (auto prod=gram.begin();prod<gram.end();prod++) {
		string rhs = prod->second;
		set<char> next_list;
		bool isfinish = false;
		for (auto& c : rhs) {
			if (non_terms.find(c) == non_terms.end()) {
				if (c != 'e') {
					next_list.insert(c);
					isfinish = true;
					break;
				}
				continue;
			}
			// we get non terms c
			set<char> copy(firsts[c]);
			if (copy.find('e') != copy.end()) {
				copy.erase('e');
				next_list.insert(copy.begin(), copy.end());
			}
			else {
				next_list.insert(copy.begin(), copy.end());
				isfinish = true;
				break;
			}
		}
		// If reach the end of rhs, add follow to next list
		if (!isfinish) {
			next_list.insert(follows[prod->first].begin(), follows[prod->first].end());
		}
		for (auto& c : next_list) {
			int row = distance(non_terms.begin(), non_terms.find(prod->first));
			int col = distance(terms.begin(), terms.find(c));
			int prod_num = distance(gram.begin(), prod);
			if (parsingtable[row][col] != -1) {
				cerr << "initParsingTable: Collision at " << getproductionbyid(parsingtable[row][col])
					<< " and " << getproductionbyid(prod_num) << endl;
				exit(-1);
			}
			parsingtable[row][col] = prod_num;
		}
	}
	cout << "Construct parsing table successfully! The result is :" << endl;
	cout << '\t';
	for (auto& m : terms) {
		cout << m << ' ';
	}
	cout << endl;
	for (auto rit = non_terms.begin(); rit != non_terms.end(); rit++) {
		cout << *rit << '\t';
		for (int col = 0; col < terms.size(); col++) {
			int row = distance(non_terms.begin(), rit);
			if (parsingtable[row][col] == -1) {
				cout << "- ";
			}
			else {
				cout << parsingtable[row][col] << ' ';
			}
		}
		cout << endl;
	}
}

string parser::getproductionbyid(int index) {
	string production;
	production = gram[index].first + infer + gram[index].second;
	return production;
}
