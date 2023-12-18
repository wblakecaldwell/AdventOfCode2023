#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/hash/hash.h"
#include <fstream>
#include <map>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

struct SourceDestMap {
  int64_t dest_start;
  int64_t source_start;
  int64_t range;

  void ToString() {
    std::cout << "source_start: " << source_start << "; dest_start: " << dest_start << "; range: " << range << std::endl;
  }
};

struct MappingBlock {
  std::vector<SourceDestMap> sorted_mappings;
  int64_t FindDest(int64_t source) {

    for(SourceDestMap m : sorted_mappings) {
      if(source >= m.source_start && source <= m.source_start + m.range) {
        return source - m.source_start + m.dest_start;
      }
    }
    return source;
  }
};

void AdvanceToEmptyLine(std::ifstream& file) {
  std::string line;
  while (std::getline(file, line) && !line.empty()) {}
}

// Parses space-separated int64_ts from a line - assumes valid format
std::vector<int64_t> Parseint64_ts(std::string line) {
  std::stringstream ss(line);
  std::vector<int64_t> numbers;
  int64_t number;
  while (ss >> number) {
    numbers.push_back(number);
  }
  return numbers;
}

std::vector<int64_t> ParseSeedLine(std::ifstream& file) {
  std::string line;
  // Read and discard up to the first space
  if (!std::getline(file, line, ' ')) {
    std::cout << "Couldn't get line up till space\n";
    exit(1);
  }
  // grab the rest of the line, and split on spaces
  if (!std::getline(file, line)) {
    std::cout << "Couldn't get rest of line with nums\n";
    exit(1);
  }
  return Parseint64_ts(line);
}

// parses a source/dest mapping block, sorting the values by source
MappingBlock ParseMappingBlock(std::ifstream& file) {
  std::string line;
  // advance past the block title
  while (line.back() != ':') {
    if (!std::getline(file, line)) {
      std::cout << "ParseMappingBlock 1\n";
      exit(1);
    }
  }

  std::vector<SourceDestMap> mappings;
  while (std::getline(file, line) && !line.empty()) {
    std::vector<int64_t> nums = Parseint64_ts(line);
    if (nums.size() != 3) {
      std::cout << "Line wasn't 3 int64_ts long: " << line << ": " << nums.size() << "\n";
      exit(1);
    }
    mappings.push_back({ nums[0], nums[1], nums[2] });
  }
  std::sort(mappings.begin(), mappings.end(), [](const SourceDestMap& a, const SourceDestMap& b)
    {
      return a.source_start < b.source_start;
    });
  return MappingBlock{mappings};
}

int64_t ProcessFile(const std::string& filename) {
  std::ifstream file(filename);

  // get the seeds
  std::vector<int64_t> seeds = ParseSeedLine(file);
  AdvanceToEmptyLine(file);

  std::vector<MappingBlock> all_mappings;

  // get the mappings
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));

  int64_t lowest_location = -1;
  for(int64_t seed : seeds) {
    int64_t source = seed;

    // follow through the mappings
    for(auto mapping_block : all_mappings) {
      source = mapping_block.FindDest(source);
      if (source == -1) {
        std::cout << "Couldn't find match for seed: " << seed << std::endl;
        exit(1);
      }
    }
    if(lowest_location == -1 || source < lowest_location) {
      lowest_location = source;
    }
  }
  return lowest_location;
}

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  std::cout << "Loading: " << file_path << std::endl;
  int64_t lowest_location = ProcessFile(file_path);
  std::cout << "Lowest location: " << lowest_location << std::endl;
  return 0;
}
