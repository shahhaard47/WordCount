#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <set>
#include <istream>

using namespace std;

int total_lines = 0, total_words = 0, total_chars = 0;

//
map<string, map <string, vector<int> > > word_search; 
map<char, map <string, vector<int> > > char_search;

map<string, int > files;

map<istream*, string> stream_filename;

//buffer to put inside the char/word search maps
vector<string> temp_wsearch;
vector<char> temp_csearch;

void write_out(string word, int line, int file, string label);
void write_out(char c, int line, int word, int file, string label);
void write_out(int lines, int words, int chars, string filename);
void write_out(int lines, int words, int chars);
bool text_file(string name);
bool flag(string arg);
void read_file(string arg, bool search, bool user_entered);
void delete_duplicates();
void print_search();
void initialize_file(string arg, bool user_entered);
void standard_input();

int main(int argc, char * argv[]){
	if (argc == 1){
		standard_input();
		exit(0);
	} 
	for (int i = 1; i<argc; i++){
		string arg = argv[i];
		if ( !(arg[0] == '-') ){ //not a flag = a potential file
			initialize_file(arg, false); //false: !stdin
		}
		else { //arg[0] == '-' must be flag
			if ( flag(arg) ){
				string flagType = arg.substr(1,8);
				if (flagType == "findword"){
					string word = arg.substr(10);
					temp_wsearch.push_back(word);
				}
				else{ //flag is "findchar"
					char s_char = arg[10];
					temp_csearch.push_back(s_char);
				}
			}else {
				exit(1);
			}	
		}
		if (i == argc-1){
			//final outputs because last argument
			if (files.size() == 0) standard_input();
			print_search();
		}
	}
}
/*HANDLE CASE WHERE FLAGS ARE ENTERED AFTER ENTERING FILES*/

//overload write_out for different purposes
int colwidth = 12;
void write_out(string word, int line, int file, string label){
	word += ":";
	cout << word;
	cout << setw(colwidth) << right << line;
	cout << setw(colwidth) << right << file;
	cout << " " << label   << endl;
}
void write_out(char c, int line, int word, int file, string label){
	cout << right << c << ":";
	cout << setw(colwidth) <<  right << line;
	cout << setw(colwidth) << right << word;
	cout << setw(colwidth) << right << file;
	cout << " " << label << endl;
}
void write_out(int lines, int words, int chars, string filename){
	cout << setw(colwidth) << right<< lines ;
	cout << setw(colwidth) << right << words ;
	cout << setw(colwidth) << right << chars ;
	cout << " "<< filename << endl;
}
void write_out(int lines, int words, int chars){
	cout << setw(colwidth) << right<< lines ;
	cout << setw(colwidth) << right << words ;
	cout << setw(colwidth) << right << chars << " " << endl;
}

//checks if the file is .txt type
bool text_file(string name){
	int dot = name.find(".");
	if (dot < 0) return false;
	else if (name.substr(dot) == ".txt"){
		return true;
	}
	return false;
}

//checks if the flag is valid
// OPTIONS: -findword and/or -findchar
bool flag(string arg){
	if (arg.length() <= 9 || (int)(arg.find('=')) < 0){
		cout << "Argument " << arg << " is poorly formed" << endl;
		return false;
	}
	string flag = arg.substr(1,8);
	if ( !(flag == "findchar" || flag == "findword") ){
		cout << "Argument " << arg << " is not recognized" << endl;
		return false;
	}
	else if (flag == "findchar"){
		//if char entered length will be -findchar=x --> 11
		if (arg.length() == 11) return true;
		else {
			cout << "Argument " << arg << " is poorly formed" << endl;
			return false;
		}
	}

	return true;
}

/****************************************************
read the file and print lines, words, and characters
update total_lines, total_words, and total_characters
update word_search and char_search maps
@param string arg = filename (unchecked for filenotfound)
@param bool search = true (file is unique, also search words and chars inside maps), 
	false (not unique)
@param bool user_entered = self explanatory
	treat the output differently if true (don't print out filename)
****************************************************/
void read_file(istream &in, bool search, bool user_entered){
	int lines = 0; //counter: number of lines in file
	int words = 0;
	int chars = 0;
	string line; //line in the text file

	while (getline(in, line)){
		
		lines++;

		chars+=line.length() + 1;

		//words
		stringstream line_stream ;//line;
		line_stream << line;
		string word;
		while ( line_stream >> word ){
			words++;
		}
		if (search){
			delete_duplicates();
			for (string s : temp_wsearch){
				//0-line, 1-file
				/*if ((int)(line.find(s)) != string::npos)
					word_search[s][arg][0]++;*/
				bool found_line = false;
				stringstream newstream;
				newstream << line;
				string w;
				while (newstream >> w){
					if (s == w){
						word_search[s][stream_filename[&in]][1]++;
						found_line = true;
					}
				}
				if (found_line){
					word_search[s][stream_filename[&in]][0]++;
				}

			}
			for (char c : temp_csearch){
				//0-line, 1-words, 2-file
				if ((int)(line.find(c)) != string::npos){ 
					//0-lines
					char_search[c][stream_filename[&in]][0]++;
					//1-words
					stringstream ss;
					ss << line;
					string w;
					while ( ss >> w){
						if (w.find(c) != string::npos) 
							char_search[c][stream_filename[&in]][1]++;
					}
					//2-file
					int found = line.find(c);
					while (found != string::npos || found < line.length()){
						found = line.find(c, found+1);
						char_search[c][stream_filename[&in]][2]++;
					}
				}
			}
		}else{
			/*not needed for now*/
		}
	}
	if (line.length() != 0) {
		lines--;
		chars--;
	}
	if (user_entered){
		write_out(lines, words, chars); //int, int, int
	}
	else{
		write_out(lines, words, chars, stream_filename[&in]); //int, int, int, string
	}
	total_lines += lines;
	total_words += words;
	total_chars += chars;
}

void delete_duplicates(){
	set<string> set_word (temp_wsearch.begin(), temp_wsearch.end());
	temp_wsearch.clear();
	set<string>::iterator it;
	for (it = set_word.begin(); it != set_word.end(); it++){
		temp_wsearch.push_back(*it);
	}

	set<char> set_char (temp_csearch.begin(), temp_csearch.end());
	temp_csearch.clear();
	set<char>::iterator at;
	for (at = set_char.begin(); at != set_char.end(); at++){
		temp_csearch.push_back(*at);
	}
}

void print_search(/*map<string, map<string, vector<int> > > wordsearch, map<char, int> charsearch*/){
	//print file total if more than one file
	map<string, int>::iterator single_iterator;
	single_iterator = files.begin();
	int first_file_value = single_iterator->second;
	if (files.size() > 1 || first_file_value > 0){
		string total = "totals";
		write_out(total_lines, total_words, total_chars, total);
	}
	
	//delete_duplicates(); //called inside read_file
	
	int total_numlines, total_numinfile; //needed in both char and word
	//temp_wsearch in alphabetical order
	//print unique findword
	int total_numwords;
	for (char loop_char : temp_csearch){
		total_numwords = 0;
		total_numlines = 0;
		total_numinfile = 0;
		//iteration
		map<string, int>::iterator it;
		for (it = files.begin(); it != files.end(); it++){
			string filename = it->first;
			int numlines = char_search[loop_char][filename][0];
			total_numlines += numlines;
			//word
			int numwords = char_search[loop_char][filename][1];
			total_numwords += numwords;
			//file
			int numinfile = char_search[loop_char][filename][2];
			total_numinfile += numinfile;

			//OUTPUTFORMAT?
			write_out(loop_char, numlines, numwords, numinfile, filename);
		}
		if (files.size() > 1){
			string total = "totals";
			write_out(loop_char, total_numlines, total_numwords, total_numinfile, total);
		}
	}
	//temp_csearch in alphabetical order
	//print unique findchar
	for (string word : temp_wsearch){
		total_numlines = 0;
		total_numinfile = 0;
		//iteraten
		map<string, int>::iterator it;
		for (it = files.begin(); it != files.end(); it++){
			string filename = it->first;
			int numlines = word_search[word][filename][0];
			total_numlines += numlines;

			int numinfile = word_search[word][filename][1];
			total_numinfile += numinfile;

			//OUTPUT FORMAT?
			write_out(word, numlines, numinfile, filename);
		}
		//PRINT total
		if (files.size() > 1){
			string total = "totals";
			write_out(word, total_numlines, total_numinfile, total);
		}
		
	}	
}

void initialize_file(string arg, bool user_entered){//(filename, stdin?)
	ifstream in;
	in.open(arg);
	stream_filename[&in] = arg;
	if ( !(in.is_open()) ){
		cout << "File " << arg << " is not found" << endl;
		//return;
		exit(1);
	}
	if ( text_file(arg) && files.find(arg) == files.end()){ 
		//new unique file
		files[arg] = 0;
		
		//add file to the map of word_search
		vector<string>::iterator st;
		for (st = temp_wsearch.begin(); st != temp_wsearch.end(); st++){
			word_search[*st][arg].push_back(0);//[0] number of lines
			word_search[*st][arg].push_back(0);//[1] number in the entire file 
		}
		vector<char>::iterator ch;
		for (ch = temp_csearch.begin(); ch != temp_csearch.end(); ch++){
			char_search[*ch][arg].push_back(0);//[0] lines
			char_search[*ch][arg].push_back(0);//[1] words
			char_search[*ch][arg].push_back(0);//[2] file
		}
		if (user_entered)
			read_file(in, true, true); //unique and stdin
		else
			read_file(in, true, false); //unique and cmdln
	}
	else if ( text_file(arg) && !(files.find(arg) == files.end())){
		//file repeated
		files[arg]++; 
		if (user_entered)
			read_file(in, false, true); //repeat and stdin
		else
			read_file(in, false, false); //repeat and cmdln
	}
	else{
		//cout << arg << ": Not a text file" << endl;
		cout << "File " << arg << " is not found" << endl;
		exit(1);
	}
}

void standard_input(){
	/*stringstream ss;
	string inputline;
	getline(cin, inputline);
	string oneWord; //potential file
	ss << inputline;*/
	//std::istream userStream (std::cin);
	read_file(cin, false, true);
	/*while (ss >> oneWord){
		read_file(cin, false, true);//true: stdin
	}*/
	print_search();
}








