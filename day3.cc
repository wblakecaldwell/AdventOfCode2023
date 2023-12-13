// by day 3, these are starting to get sloppy! ¯\_(ツ)_/¯ ...
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/hash/hash.h"
#include <fstream>
#include <map>
#include <cstdlib>
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

// Item represents either a symbol or number, holding its row, column start/finish, value,
// and whether it's a symbol, and if so, if it's a gear.
struct Item {
  int row;
  int column_start;
  int column_end;
  int value; // -1 for symbol
  bool is_gear = false;
  Item(int row)
    : row(row), column_start(-1), column_end(-1), value(0)
  {}
  Item(int row, int column_start, int column_end, int value)
    : row(row), column_start(column_start), column_end(column_end), value(value)
  {}
  bool IsNumber() {
    return value > 0;
  }
  bool IsSymbol() {
    return value == -1;
  }
  // Checks if this is a number that's adjacent to the input item,
  // which is a symbol.
  bool IsNumberAdjacentToSymbol(Item& symbol) {
    if (!IsNumber() || !symbol.IsSymbol() || abs(row - symbol.row) > 1) {
      return false;
    }
    return
      (symbol.column_start >= column_start - 1) && (symbol.column_start <= column_end + 1);
  }
  // overload less-than operator for std::map keying
  bool operator<(const Item& other) const {
    if (row < other.row) return true;
    if (row > other.row) return false;

    if (column_start < other.column_start) return true;
    if (column_start > other.column_start) return false;

    if (column_end < other.column_end) return true;
    if (column_end > other.column_end) return false;

    if (value < other.value) return true;
    if (value > other.value) return false;

    return is_gear < other.is_gear;
  }
  void SetIsGear() {
    is_gear = true;
  }
  bool IsGear() {
    return is_gear;
  }
};

// Represents metadata about a gear, including how many neighboring numbers were found,
// and the ratio, if match_count is 2.
struct GearRatio {
  int match_count = 0;
  // holds the ratio, but only a valid ratio if match_count==2. If 1, just holds first match value, if >2, holds 0.
  int ratio;

  // Registers a match, updating match_count and the gear ratio.
  // - if this is the first match, we store the value
  // - if this is the second match, we find the gear ratio
  // - if we've matched more than twice, we set ratio to 0
  void ConsumeMatch(int value) {
    match_count++;
    if (match_count == 1) {
      ratio = value;
    } else if (match_count == 2) {
      ratio *= value;
    } else {
      ratio = 0;
    }
  }
};

// Parses a line, returning a vector of Items.
std::vector<Item> ParseLine(absl::string_view line, int row) {
  std::vector<Item> ret;
  Item current_item(row);
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
        current_item = Item(row);
      }
      if (c != '.') {
        // symbol
        Item new_item(row, i, i, -1);
        if (c == '*') {
          new_item.SetIsGear();
        }
        ret.push_back(new_item);
      }
    }
  }
  if (current_item.column_start > -1) {
    ret.push_back(current_item);
  }
  return ret;
}

// Loops over each line, keeps track of operators and numbers from this and the previous line.
// Checks every number in that list against every operator. All operators from the current line,
// and all numbers that haven't matched are carried into the next row's collection for comparison.
void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;
  int sum = 0;
  
  // info about the gears we find, so we can keep track of the ratio, if a gear touches 2 numbers.
  std::map<Item, GearRatio> gears;

  // from previous line: numbers that haven't had a match yet; all symbols
  std::vector<Item> prev_line_items;
  int row = 0;
  // each line
  while (std::getline(file, line)) {
    // parse this line
    std::vector<Item> items = ParseLine(line, row);

    // group all items to be considered into one group, and inefficiently compare them all
    std::vector<Item> carry_over;
    prev_line_items.insert(prev_line_items.end(), items.begin(), items.end());
    for (Item number : prev_line_items) {
      bool number_matched = false;
      if (number.IsNumber()) {
        for (Item symbol : prev_line_items) {
          if (symbol.IsSymbol()) {
            if (symbol.row == row) {
              carry_over.push_back(symbol);
            }
            if (number.IsNumberAdjacentToSymbol(symbol)) {
              if (symbol.IsGear()) {
                if (auto it = gears.find(symbol); it != gears.end()) {
                  // alrady exists - update it
                  GearRatio ratio = it->second;
                  ratio.ConsumeMatch(number.value);
                  gears[symbol] = ratio;
                } else {
                  // first match for this gear
                  GearRatio ratio{};
                  ratio.ConsumeMatch(number.value);
                  gears[symbol] = ratio;
                }
              }
              sum += number.value;
              number_matched = true;
              break;
            }  // IsSymbol
          }
        }  // each symbol item
        if (!number_matched && number.row == row) {
          carry_over.push_back(number);
        }
      }  // IsNuber
    } // each number item

    prev_line_items = carry_over;
    row++;
  } // each line

  // figure out and add up all the gear ratios
  int gear_ratio_sums = 0;
  for (auto it = gears.begin(); it != gears.end(); ++it) {
    if (it->second.match_count == 2) {
      gear_ratio_sums += it->second.ratio;
    }
  }

  std::cout << std::endl;
  std::cout << "Sum: " << sum << "\n";
  std::cout << "Gear Ratio Sum: " << gear_ratio_sums << "\n";
}