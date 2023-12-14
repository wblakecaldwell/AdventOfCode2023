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
  std::cout << "Loading: " << file_path << std::endl;
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
  int part_1_score = 0;
  int part_2_score = 0;

  // Reused index - for each line, set the row number to the index at each winning number.
  // When checking our number, we just check if the value at the number is the row number.
  // This way, we don't have to clear out the array after each line.
  int matches[100] = {};
  std::vector<int> additional_future_cards;
  while (std::getline(file, line)) {
    row_num++;

    // find out how many cards we have for this row
    int card_count = 1;
    if (additional_future_cards.size() > 0) {
      card_count += additional_future_cards.at(0);
      additional_future_cards.erase(additional_future_cards.begin());
    }

    // parse the line for match count
    int match_count = ParseLine(line, row_num, matches);

    // part 1 score is based on a points system
    part_1_score += match_count > 0 ? pow(2, match_count - 1) : 0;

    // part 2 score is just card counts
    part_2_score += card_count;

    // add an additional card to each successive card based on number of matches
    for (int i = 0; i < match_count; i++) {
      if (i < additional_future_cards.size()) {
        additional_future_cards[i] += card_count;
      } else {
        additional_future_cards.push_back(card_count);
      }
    }

    std::cout << "row: " << row_num << "; card_count: " << card_count << "; match_count: " << match_count << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Part 1 score: " << part_1_score << std::endl;
  std::cout << "Part 2 score: " << part_2_score << std::endl;
}