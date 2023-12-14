#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/hash/hash.h"
#include <fstream>
#include <map>
#include <cstdlib>
#include <cmath>
#include <vector>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

void ProcessFile(const std::string& filename);

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  ProcessFile(file_path);
  return 0;
}

// Parses a line, returning match count
int ParseLine(absl::string_view line, int row_num, int matches[100]) {
  int match_count = 0;
  bool seen_colon = false;
  bool seen_bar = false;
  for (int i = 0; i < line.length(); i++) {
    char c = line.at(i);
    if (!seen_colon) {
      if (c == ':') {
        seen_colon = true;
      }
      continue;
    }
    if (!seen_bar) {
      if (c == '|') {
        seen_bar = true;
        continue;
      }
      // collecting the winning numbers
      int num = 0;
      do {
        if (c >= '0' && c <= '9') {
          num = num * 10 + (c - '0');
        }
        i++;
        c = line.at(i);
      } while (c != ' ' && c != '|');
      if (num > 99) {
        std::cout << "Found big number when parsing winning numbers: " << num << std::endl;
        exit(-1);
      }
      if (num > 0) {
        matches[num] = row_num;
      }
      continue;
    }
    // checking our numbers against winning numbers
    int num = 0;
    do {
      if (c >= '0' && c <= '9') {
        num = num * 10 + (c - '0');
      }
      i++;
      if (i == line.length()) {
        break;
      }
      c = line.at(i);
    } while (c != ' ');
    if (num > 99) {
      std::cout << "Found big number when parsing my numbers: " << num << std::endl;
      exit(-2);
    }
    if (num > 0 && matches[num] == row_num) {
      match_count++;
    }
  }
  return match_count;
}

void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;
  int row_num = 0;
  int sum = 0;

  // Reused index - for each line, set the row number to the index at each winning number.
  // When checking our number, we just check if the value at the number is the row number.
  // This way, we don't have to clear out the array after each line.
  int matches[100] = {};
  while (std::getline(file, line)) {
    row_num++;
    if (int match_count = ParseLine(line, row_num, matches); match_count > 0) {
      sum += pow(2, match_count-1);
    }
  }

  std::cout << std::endl;
  std::cout << "Sum: " << sum << "\n";
}