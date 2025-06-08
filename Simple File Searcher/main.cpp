#include "KMP_algorythm.h"
#include <filesystem>
#include <string>
#include <fstream>
#include <shlwapi.h>
using namespace std::filesystem;
using namespace std;

enum Flag {
	REPLACE				   = 1,
	DEL					   = 2,
	PRINTMATCHES		   = 4,
	FILEPATHSUBMITTED	   = 8,
	NEEDREPLACESTRING	   = 32,
	NEEDSEARCHINFO		   = 64,
	SHOWUSAGEINFO		   = 128,
	WPATHCANCELED		   = 256,
	WPATHUSED			   = 512,
	LIMITSEARCH			   = 1024,
	LIMITREPLACE		   = 2048,
	NEEDSEARCHLIMITNUMBER  = 4096,
	NEEDREPLACELIMITNUMBER = 8192,
	ONEMATCH			   = 16384
};

enum Miniflag {
	BADNUMBER = 1,
};

static void GetWPATHFromUser(wstring& wpath, uint16_t flags) {
	bool firsttime = true;
path:
	cout << (firsttime ? "You chose using wpath in the arguments, so you need to enter the path manually.\nPlease enter the file path: " : "Please enter a valid path OR press ENTER to use the normal path: ");
	getline(wcin, wpath);
	if (wpath.empty()) {
		if (firsttime) {
			firsttime = false;
			goto path;
		}
		else {
			flags |= WPATHCANCELED;
			return;
		}
	}
	if (wpath.size() < 4) {
		firsttime = false;
		goto path;
	}
	flags |= WPATHUSED;
}

static void CheckStringForOnlyNum(string &str) {
	bool badnumber = false;
	size_t str_len = str.size();

	if (str.empty() || str_len > 19) badnumber = true;

	if (!badnumber) {
		for (size_t i = 0; i < str_len; i++) {
			char c = str[i];
			if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9') {
				badnumber = true;
			}
		}
	}

	while (badnumber) {
		cout << "Please enter a valid unsigned limit number less than (10 pow 19): ";
		getline(cin, str);
		str_len = str.size();

		if (str.empty() || str_len > 19) {
			continue;
		}
		for (size_t i = 0; i < str_len; i++) {
			char c = str[i];
			if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9') {
				continue;
			}
		} return;
	}
}


static bool Choice() {
	std::string Response;
	std::getline(std::cin, Response);
	if (Response[0] != 'y' && Response[0] != 'Y' && Response[0] != 'n' && Response[0] != 'N') {
		do {
			std::cout << "Enter a valid input [y/N]: ";
			std::getline(std::cin, Response);
		} while (Response.empty() || (Response[0] != 'y' && Response[0] != 'Y' && Response[0] != 'n' && Response[0] != 'N'));
	}

	if (Response[0] == 'y' || Response[0] == 'Y') {
		return true;
	}
	else {
		return false;
	}
}

static void MainInputFromUser(string& replacestring, size_t& searchlimit, size_t& replacelimit, uint16_t& flags) {
	// Replace
	cout << "Do you want to replace the pattern found? [y/N]: ";
	if (Choice()) {
		flags |= REPLACE;
		cout << "Please enter your replacement string: ";
		do getline(cin, replacestring); while (replacestring.empty());
	}
	// Delete
	if (!(flags & REPLACE)) {
		cout << "Do you want to delete the pattern found? [y/N]: ";
		if (Choice()) flags |= DEL;
	}
	// R/D Limit
	if ((flags & REPLACE) || (flags & DEL)) {
		cout << "Do you want to set a " << ((flags & REPLACE) ? "replace" : "delete") << " limit? [y/N]: ";
		if (Choice()) {
			flags |= LIMITREPLACE;
			cout << "Please enter your " << ((flags & REPLACE) ? "replace" : "delete") << " limit number: ";
			string strlimit;
			do getline(cin, strlimit); while (strlimit.empty());
			CheckStringForOnlyNum(strlimit);
			replacelimit = stoull(strlimit);
		}
	}
	// Search limit
	cout << "Do you want to set a search limit? [y/N]: ";
	if (Choice()) {
		flags |= LIMITSEARCH;
		cout << "Please enter your search limit number: ";
		string strlimit;
		do getline(cin, strlimit); while (strlimit.empty());
		CheckStringForOnlyNum(strlimit);
		searchlimit = stoull(strlimit);
	}
	// Print matches
	cout << "Do you want to print the array of match positions? [y/N]: ";
	if (Choice()) flags |= PRINTMATCHES;
}

static string CharArrToStringCheckNum(char* strold, bool SearchOrReplace) {
	bool badnumber = false;
	string str = string(strold);
	size_t str_len = str.size();

	if (str.empty() || str_len > 19) badnumber = true;

	if (!badnumber) {
		for (size_t i = 0; i < str_len; i++) {
			char c = str[i];
			if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9') {
				badnumber = true;
			}
		}
	}

	while (badnumber) {
		cout << "Please enter a valid unsigned " << (SearchOrReplace ? "Search " : "Replace ") << "limit number less than (10 pow 19): ";
		getline(cin, str);
		str_len = str.size();

		if (str.empty() || str_len > 19) {
			continue;
		}
		for (size_t i = 0; i < str_len; i++) {
			char c = str[i];
			if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9') {
				continue;
			}
		} return str;
	}
}

static string ReformatString(string& str) {
	if (str.empty()) return "";
	string result;
	size_t strsize = str.size();
	size_t curpos = 0;
	for (size_t i = 0; i < strsize; i++) {
		if (str[i] == '\\' && i + 1 < strsize) {
			result.append(str, curpos, i - curpos);
			if (str[i + 1] == '\\') {
				result += '\\';
				curpos = ++i + 1;
			}
			else {
				switch (str[i + 1]) {
				case 'n': result += '\n'; break;
				case 't': result += '\t'; break;
				case 'r': result += '\r'; break;
				default:
					result += '\\';
					curpos = ++i;
					continue;
				}
				curpos = ++i + 1;
			}
		}
	}
	result.append(str, curpos, strsize - curpos);
	return result;
}

static void GetWPathAndPattern(wstring& wpath, string& patterntofind, uint16_t& flags) {
	bool firsttime = true;
path:
	cout << (firsttime ? "Please enter the file path: " : "Please enter a valid path: ");
	firsttime = false;
	getline(wcin, wpath);

	if (wpath.size() < 4) {
		goto path;
	}

	if (wpath.empty()) {
		goto path;
	}

	firsttime = true;
pattern:
	cout << (firsttime ? "Now, please enter your pattern to find: " : "Please enter a valid pattern: ");
	firsttime = false;
	getline(cin, patterntofind);

	if (patterntofind.empty()) {
		goto pattern;
	}
}

static uint8_t ArgsManagement(int& argc, char* argv[], string& replacestring, string& path, wstring& wpath, string& patterntofind, size_t& searchlimit, size_t& replacelimit, uint16_t& flags) {
	for (int i = 1; i < argc; i++) {
		if (flags & NEEDSEARCHINFO) {
			if (flags & FILEPATHSUBMITTED) {
				patterntofind = string(argv[i]);
				flags &= ~NEEDSEARCHINFO;
			}
			else {
				path = string(argv[i]);
				flags |= FILEPATHSUBMITTED;
				if (!((i + 1) < argc)) goto errorcheck;
			}
			continue;
		}
		if (flags & NEEDREPLACESTRING) {
			replacestring = argv[i];
			flags &= ~NEEDREPLACESTRING;
			continue;
		}
		if (flags & NEEDSEARCHLIMITNUMBER) {
			searchlimit = stoull(CharArrToStringCheckNum(argv[i], true));
			flags &= ~NEEDSEARCHLIMITNUMBER;
			continue;
		}
		if (flags & NEEDREPLACELIMITNUMBER) {
			replacelimit = stoull(CharArrToStringCheckNum(argv[i], false));
			flags &= ~NEEDREPLACELIMITNUMBER;
			continue;
		}
		if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-S") == 0 && !(flags & FILEPATHSUBMITTED)) {
			flags |= NEEDSEARCHINFO;
			if (!((i + 2) < argc)) goto errorcheck;
			continue;
		}
		if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-R") == 0 && !(flags & REPLACE)) {
			flags |= REPLACE; flags |= NEEDREPLACESTRING;
			if (!((i + 1) < argc)) goto errorcheck;
			continue;
		}
		if (strcmp(argv[i], "-l") == 0 && !(flags & LIMITREPLACE)) {
			flags |= LIMITREPLACE; flags |= NEEDREPLACELIMITNUMBER;
			if (!((i + 1) < argc)) goto errorcheck;
			continue;
		}
		if (strcmp(argv[i], "-L") == 0 && !(flags & LIMITSEARCH)) {
			flags |= LIMITSEARCH; flags |= NEEDSEARCHLIMITNUMBER;
			if (!((i + 1) < argc)) goto errorcheck;
			continue;
		}
		if (argv[i][0] == '-') {
			for (int j = 1; argv[i][j] != '\0'; j++) {
				switch (argv[i][j]) {
				case 'h':
				case 'H':
					flags |= SHOWUSAGEINFO;
					break;
				case 'd':
				case 'D':
					flags |= DEL;
					break;
				case 'p':
				case 'P':
					flags |= PRINTMATCHES;
					break;
				case 'w':
				case 'W':
					if (!(flags & WPATHUSED)) {
						GetWPATHFromUser(wpath, flags);
					}
					break;
				default:
					break;
				}
			}
		}

	}
errorcheck:
	if (flags & NEEDREPLACESTRING) {
		cout << "Bad usage of replace! Valid usage: -r [ReplacementString]. Add -h / -H while starting the program to see the usage info.\n\n";
		cout << "Press ENTER to exit..";
		cin.get();
		return 15;
	}
	if (flags & NEEDSEARCHINFO) {
		cout << "Bad usage of arguments! Valid usage: -s [PathToFile] [PatternToSearchInFile]. Add -h / -H while starting the program to see the usage info.\n\n";
		cout << "Press ENTER to exit..";
		cin.get();
		return 16;
	}
	if (flags & NEEDREPLACELIMITNUMBER || flags & NEEDSEARCHLIMITNUMBER) {
		cout << "Bad usage of arguments! Valid usage: -l / -L [Limit number]. Add -h / -H while starting the program to see the usage info.\n\n";
		cout << "Press ENTER to exit..";
		cin.get();
		return 17;
	}

	if (flags & SHOWUSAGEINFO) {
		cout << "Usage: " << argv[0] << " (-s [Path to file] [PatternToSearchInFile]) [flags] or no arguments.\n\n";
		cout << "Flags:\n-r / -R [ReplacementString]: Replaces the found pattern by [ReplacementString] in the file.\n";
		cout << "-w / -W: Use the wstring for the path (use if your path contains other languages)\n";
		cout << "-L [Limit founds number]: Gives a limit how much the program can maximum find the pattern then it will stop searching.\n";
		cout << "-l [Limit replace/delete number]: Gives a limit how much patterns found the program can maximum replace or delete.\n";
		cout << "-d / -D: Deletes the found pattern in the file, if -r / -R is active it will change nothing.\n";
		cout << "-p / -P: Prints the array with match positions.\n\n";
		cout << "Example usage: " << argv[0] << " -s C:/Users/Username/Downloads/text.txt ExampleOfPatternToFind -p -r SomeRandomReplacementString\n\n";
		cout << "Press ENTER to exit..";
		cin.get();
		return 1;
	}
	return 0;
}



int main(int argc, char* argv[])
{
	setlocale(LC_CTYPE, "");
	// Args management
	string replacestring;
	string path; wstring wpath;
	string patterntofind;
	size_t searchlimit = 0;
	size_t replacelimit = 0;
	uint16_t flags = 0;

	if (argc > 1)
	{
		uint8_t ArgsResult = ArgsManagement(argc, argv, replacestring, path, wpath, patterntofind, searchlimit, replacelimit, flags);
		if (ArgsResult != 0) return 4;
	}
	// Args management
	if (path.empty()) {
		GetWPathAndPattern(wpath, patterntofind, flags);
		flags |= WPATHUSED;
	}

	if (flags & WPATHUSED) wpath.erase(remove(wpath.begin(), wpath.end(), '\"'), wpath.end());
	else path.erase(remove(path.begin(), path.end(), '\"'), path.end());

	::path p(path);
	::path wp(wpath);
	if (!(exists(((flags & WPATHUSED) ? wp : p)) && is_regular_file(((flags & WPATHUSED) ? wp : p)))) {
		cout << "The path isnt a file! Path Example: C:/Users/Username/Downloads/text.txt\nAdd -h / -H while starting the program to see the usage info.\n\n";
		cout << "Press ENTER to exit..";
		cin.get();
		return 5;
	}

	ifstream file;
	if (flags & WPATHUSED) file.open(wpath, ios::binary | ios::ate);
	else file.open(path, ios::binary | ios::ate);

	if (!file.is_open()) {
		cout << "Error open file to read!\n\nPress ENTER to exit..";
		cin.get();
		return 10;
	}

	if (argc == 1) {
		MainInputFromUser(replacestring, searchlimit, replacelimit, flags);
	}

	// Initialisation
	vector<char> data(file.tellg());
	file.seekg(0);
	file.read(data.data(), data.size());
	file.close();

	vector<size_t> matcharray;
	vector<size_t> lps;
	string pattern = ReformatString(patterntofind);
	// deleteFlag  = flags & DEL
	// replaceFlag = flags & REPLACE
	// PrintFlag   = flags & PRINTMATCHES
	string replacement = ReformatString(replacestring);

	ComputeLPS(pattern, lps);

	size_t foundcount = search(
		data, matcharray, lps, pattern,
		(flags & DEL), (flags & REPLACE), (flags & PRINTMATCHES),
		(flags & LIMITSEARCH), searchlimit,
		(flags & LIMITREPLACE), replacelimit, replacement);

	if (foundcount == 0) {
		cout << "\nPattern not found!\n\nPress ENTER to exit..";
		cin.get();
		return 1;
	}
	if (foundcount == 1) flags |= ONEMATCH;

	cout << "\nPattern successfully found " << foundcount << ((flags & ONEMATCH) ? "time!\n" : " times!\n");

	if ((flags & DEL) || (flags & REPLACE)) {
		std::ofstream outFile;
		if (flags & WPATHUSED) outFile.open(wpath, ios::binary);
		else outFile.open(path, ios::binary);
		if (!file.is_open()) {
			cout << "Error open file to write!\n\nPress ENTER to exit..";
			cin.get();
			return 11;
		}
		outFile.write(data.data(), data.size());
		outFile.close();
		cout << "File successfully updated!\n";
	}
	if (flags & PRINTMATCHES) {
		if (flags & ONEMATCH) {
			cout << "One match at position " << matcharray[0] << ".";
			goto end;
		}
		cout << "\nMatches at positions:\n";
		size_t MA_sizeNOEND = (matcharray.size() - 1);
		for (size_t i = 0; i < MA_sizeNOEND; i++) {
			cout << matcharray[i] << ", ";
			if (i % 5 == 0) {
				cout << "\n";
			}
		} cout << matcharray[MA_sizeNOEND] << ".";
	}
end:
	return 0;
}
