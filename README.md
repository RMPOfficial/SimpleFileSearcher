# SimpleFileSearcher

## Description
This tool search for a pattern in files with the [KMP Algorythm](https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm) and can perform some features.

## Features
- Search for the pattern, can set a limit of pattern founds after what it will stop searching
- Print the array with positions of found patterns
- Replace the pattern by an other string, can set a limit
- Delete the pattern, can set a limit

# Usage
 SimpleFileSearcher.exe -s [Path to file] [PatternToSearchInFile] [flags] or no arguments.
 
*Flags:*
+ -r / -R [ReplacementString]: Replaces the found pattern by [ReplacementString] in the file.
+ -w / -W: Use the wstring for the path (use if your path contains other languages).
+ -L [Limit founds number]: Gives a limit how much the program can maximum find the pattern then it will stop searching.
+ -l [Limit replace/delete number]: Gives a limit how much patterns found the program can maximum replace or delete.
+ -d / -D: Deletes the found pattern in the file, if -r / -R is active it will change nothing.
+ -p / -P: Prints the array with match positions.

Example usage: SimpleFileSearcher-x64.exe -s C:/Users/Username/Downloads/text.txt ExampleOfPatternToFind -r SomeRandomReplacementString -P
