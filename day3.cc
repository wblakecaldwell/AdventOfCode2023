#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/status/statusor.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include <fstream>
#include <vector>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

void ProcessFile(const std::string& filename);

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  std::cout << "Loading: " << file_path << std::endl << std::endl;
  ProcessFile(file_path);
}

struct Item {
  int value; // -1 for symbol
  int column_start;
  int column_end;
};

std::vector<Item> ParseLine(absl::string_view line) {
  std::vector<Item> ret;
  Item current_item{0, -1, -1};
  for (int i = 0; i < line.length(); i++) {
    char c = line.at(i);
    if (c >= '0' && c <= '9') {
      current_item.column_start = current_item.value == 0 ? i : current_item.column_start;
      current_item.column_end = i;
      current_item.value = 10 * current_item.value + c - '0';
    } else {
      if (current_item.value > 0) {
        current_item.column_end = i - 1;
        ret.push_back(current_item);
        current_item = {0, -1, -1};
      }
      if (c != '.') {
        // symbol
        ret.push_back({-1, i, i});
      }
    }
  }
  if(current_item.column_start > -1) {
    ret.push_back(current_item);
  }
  return ret;
}

// Loops over each line:
// 1. evaluate numbers determined part number from this line alone
// 2. remember position of symbols for next line
// 3. check this line's symbols against previous line
void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;
  int sum = 0;


  std::vector<Item> prev_line_items;
  // each line
  while (std::getline(file, line)) {
    std::vector<Item> items = ParseLine(line);
    std::cout << line << std::endl;
    for(Item item : items) {
      std::cout << "\t" << (item.value > -1 ? absl::StrCat(item.value) : "*") << " from: " << item.column_start << "-" << item.column_end << std::endl;
    }
  } // each line

  std::cout << std::endl;
  std::cout << "Sum: " << sum << "\n";
}
