#pragma once

#include <iostream>
#include <vector>

#define MATCHBEGINPOS (curposdata - curpospat)

void ComputeLPS(const std::string& pattern, std::vector<std::size_t>& lps);

std::size_t search(
	std::vector<char>& data,
	std::vector<size_t>& matcharray,
	std::vector<std::size_t>& lps,
	std::string& pattern,
	bool deleteFlag,
	bool replaceFlag,
	bool printFlag,
	bool limitsearch,
	std::size_t& searchlimit,
	bool limitreplace,
	std::size_t& replacelimit,
	std::string& replacement);