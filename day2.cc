#include <iostream>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/status/statusor.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h" 
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <memory>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

void ProcessFile(const std::string& filename);

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  ProcessFile(file_path);
}

void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;

  int sum = 0;
  while (std::getline(file, line)) {
    // Extract the game ID
    size_t start = line.find("Game ") + 5; // Start after "Game "
    size_t end = line.find(':', start); // Find the colon after "Game "
    if (start == std::string::npos || end == std::string::npos) {
      continue; // No "Game " or colon found, skip the line
    }

    std::string game_id_str = line.substr(start, end - start);
    std::string game_data = line.substr(end + 1);
    std::stringstream roll_ss(game_data);
    std::string roll_item, color_item;

    int32_t game_id;
    if (!absl::SimpleAtoi(game_id_str, &game_id)) {
      continue; // If conversion fails, skip the line
    }

    bool possible = true;
    int red_count = 0;
    int green_count = 0;
    int blue_count = 0;

    // each roll
    while (possible && std::getline(roll_ss, roll_item, ';')) {
      std::cout << "Parsing roll:" << roll_item << "\n";

      // each color
      std::stringstream color_ss(roll_item);
      while (possible && std::getline(color_ss, color_item, ',')) {
        std::stringstream item_stream(color_item);
        int32_t count;
        std::string color;

        if (item_stream >> count >> color) {
          if (color == "red") {
            if (count > red_count) {
              red_count = count;
            }
          } else if (color == "green") {
            if (count > green_count) {
              green_count = count;
            }
          } else if (color == "blue") {
            if (count > blue_count) {
              blue_count = count;
            }
          }
        }
      } // each color
      if (red_count > 12 || green_count > 13 || blue_count > 14) {
        possible = false;
      }
    } // each roll

    if (possible) {
      sum += game_id;
    }
  
  } // each game

  std::cout << "Total sum: " << sum << "\n";
}
