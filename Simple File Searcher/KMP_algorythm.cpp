#include "KMP_algorythm.h"

void ComputeLPS(const std::string& pattern, std::vector<size_t>& lps) {
	size_t pat_size = pattern.size();
	lps.assign(pat_size, 0);
	if (pat_size == 1) return;
	size_t len = 0;

	for (int i = 1; i < pat_size;) {
		if (pattern[i] == pattern[len]) {
			len++;
			lps[i] = len; // = (lps[i++] = ++len;)
			i++;
		}
		else if (len > 0) {
			len = lps[len - 1];
		}
		else {
			lps[i] = 0; // To be sure that it will be 0.
			i++;
		}
	}
}

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
	std::string& replacement) {

	std::size_t datasize = data.size();
	std::size_t pat_size = pattern.size();

	std::size_t curposdata = 0;
	std::size_t curpospat = 0;

	std::size_t timesfound = 0;
	std::size_t timesRD = 0;

	std::size_t replacementsize;
	if (replaceFlag) replacementsize = replacement.size();

	while (curposdata < datasize) {
		if (limitsearch && timesfound >= searchlimit) break;
		if (data[curposdata] == pattern[curpospat]) {
			curposdata++; curpospat++;
			if (curpospat == pat_size) {
				timesfound++;
				if (printFlag) matcharray.push_back(MATCHBEGINPOS);
				if (!limitreplace || timesRD < replacelimit)
				{
					if (replaceFlag || deleteFlag) {
						timesRD++;
						data.erase(data.begin() + (MATCHBEGINPOS), data.begin() + curposdata);
						if (replaceFlag) {
							data.insert(data.begin() + (MATCHBEGINPOS), replacement.begin(), replacement.end());
							curposdata += replacementsize;
						}
						curposdata -= curpospat;
						datasize = data.size();
					}
				}
				curpospat = lps[curpospat - 1];
			}
		}
		else if (curpospat > 0) {
			curpospat = lps[curpospat - 1];
		}
		else curposdata++;
	}
	return timesfound;
}