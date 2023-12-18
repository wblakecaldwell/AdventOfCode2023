#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <fstream>
#include <sstream>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

// Represents a line of ints: [dest, source, range]
struct SourceDestMap {
  int64_t dest_start;
  int64_t source_start;
  int64_t range;

  void ToString() {
    std::cout << "source_start: " << source_start << "; dest_start: " << dest_start << "; range: " << range << std::endl;
  }
};

// Represents a block of mappings for a source type to a dest type
struct MappingBlock {
  std::string name;
  std::vector<SourceDestMap> sorted_mappings;

  void SetNameFromLine(std::string line) {
    size_t colonPos = line.rfind(':');
    if (colonPos != std::string::npos) {
      // Extract everything up to the colon
      name = line.substr(0, colonPos);
    } else {
      name = line;
    }
  }

  // Find the dest given the source
  int64_t FindDest(int64_t source) {
    for (SourceDestMap m : sorted_mappings) {
      if (source >= m.source_start && source <= m.source_start + m.range - 1) {
        return source - m.source_start + m.dest_start;
      }
    }
    return source;
  }
};

// Parses space-separated int64_ts from a line - assumes valid format
std::vector<int64_t> ParseInts(std::string line) {
  std::stringstream ss(line);
  std::vector<int64_t> numbers;
  int64_t number;
  while (ss >> number) {
    numbers.push_back(number);
  }
  return numbers;
}

// Parse the seeds line, advancing to empty line
std::vector<int64_t> ParseSeedBlock(std::ifstream& file) {
  std::string line;
  // Read and discard up to the first space
  if (!std::getline(file, line, ' ')) {
    std::cout << "ParseSeedBlock 1" << std::endl;
    exit(1);
  }
  // grab the rest of the line, and split on spaces
  if (!std::getline(file, line)) {
    std::cout << "ParseSeedBlock 2" << std::endl;
    exit(1);
  }
  // advance to empty line
  std::vector<int64_t> ret = ParseInts(line);
  while (std::getline(file, line) && !line.empty()) {}
  return ret;
}

// parses a source/dest mapping block, sorting the values by source, advancing to empty line or eof.
MappingBlock ParseMappingBlock(std::ifstream& file) {
  std::string line;
  // advance past the block title
  while (line.back() != ':') {
    if (!std::getline(file, line)) {
      std::cout << "ParseMappingBlock 1\n";
      exit(1);
    }
  }
  MappingBlock ret;
  ret.SetNameFromLine(line);

  std::vector<SourceDestMap> mappings;
  while (std::getline(file, line) && !line.empty()) {
    std::vector<int64_t> nums = ParseInts(line);
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
  ret.sorted_mappings = mappings;
  return ret;
}

void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);

  // get the seeds
  std::vector<int64_t> seeds = ParseSeedBlock(file);

  std::vector<MappingBlock> all_mappings;

  // get the mappings
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));
  all_mappings.push_back(ParseMappingBlock(file));

  auto find_location = [all_mappings](int64_t seed)
    {
      int64_t source = seed;
      // follow through the mappings
      for (auto mapping_block : all_mappings) {
        source = mapping_block.FindDest(source);
        if (source == -1) {
          std::cout << "Couldn't find match for seed: " << seed << std::endl;
          exit(1);
        }
      }
      return source;
    };

  // Part 1:
  int64_t lowest_location = -1;

  // update lowest_location if the input is lower; return input
  auto register_location = [&lowest_location](int64_t location)
    {
      if (lowest_location == -1 || location < lowest_location) {
        lowest_location = location;
      }
      return location;
    };

  for (int64_t seed : seeds) {
    register_location(find_location(seed));
  }
  std::cout << "Part 1: " << lowest_location << std::endl;

  // Part 2
  struct IntPairs {
    int64_t start;
    int64_t end;
  };
  std::vector<IntPairs> pairs;
  lowest_location = -1;
  for (int seed_pair = 0; seed_pair < seeds.size() / 2; seed_pair++) {
    int64_t start = seeds[seed_pair * 2];
    int64_t end = start + seeds[seed_pair * 2 + 1];
    if (start > end) {
      std::cout << "Start is bigger than end: start:" << start << "-" << end << std::endl;
    }
    pairs.push_back({ start, end - 1 });
  }

  // std::cout << "range: " << seed_range_start << "-" << seed_range_end << std::endl;
  for (int pair_index = 0; pair_index < pairs.size(); pair_index++) {
    int64_t start = pairs.at(pair_index).start;
    int64_t end = pairs.at(pair_index).end;

    int64_t start_location = register_location(find_location(start));
    int64_t end_location = register_location(find_location(end));

    // Note: we assume that in the ranges of seeds given to us, if their locations appears linear
    // by checking the start and end location, then they are, and we don't have to search seeds
    // within that range, since the start seed will have the lowest location. The data could be
    // arranged so that this doesn't work, but in this puzzle, it's not. So, we assume that this
    // assumption was intended to be made, since it wasn't called out in the description.
    if (end - start != end_location - start_location) {
      int64_t mid = ((end - start) / 2) + start;
      if (mid < start || mid > end) {
        std::cout << "Invalid start/mid/end: start:" << start << ", mid: " << mid << ", end: " << end << std::endl;
        exit(1);
      }
      if (mid > start + 1) {
        pairs.push_back({ start + 1, mid });
      }
      if (mid + 1 < end - 1) {
        pairs.push_back({ mid + 1, end - 1 });
      }
    }
  }
  std::cout << "Part 2: " << lowest_location << std::endl;
}

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  std::cout << "Loading: " << file_path << std::endl;
  ProcessFile(file_path);
  return 0;
}
