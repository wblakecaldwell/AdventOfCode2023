#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/status/statusor.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include <fstream>
#include <cstdlib>
#include <vector>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

void ProcessFile(const std::string& filename);

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  std::cout << "Loading: " << file_path << std::endl;
  ProcessFile(file_path);
}

struct Item {
  int row;
  int column_start;
  int column_end;
  int value; // -1 for symbol
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
};

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
        ret.emplace_back(row, i, i, -1);
      }
    }
  }
  if (current_item.column_start > -1) {
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
              sum += number.value;
              number_matched = true;
              break;
            }
          }
        }
        if (!number_matched && number.row == row) {
          carry_over.push_back(number);
        }
      } // IsNuber
    }

    prev_line_items = carry_over;
    row++;
  } // each line

  std::cout << std::endl;
  std::cout << "Sum: " << sum << "\n";
}
