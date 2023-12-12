#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/statusor.h"
#include <fstream>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

void ProcessFile(const std::string& filename);

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string file_path = absl::GetFlag(FLAGS_input_file);
  std::cout << "Loading: " << file_path << std::endl << std::endl;
  ProcessFile(file_path);
}

void ProcessFile(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;
  int line_num = 0;
  int part_1_sum = 0;
  int part_2_sum = 0;

  // each game
  while (std::getline(file, line)) {
    line_num++;
    // Extract the game ID
    size_t start = line.find("Game ") + 5; // Start after "Game "
    size_t end = line.find(':', start); // Find the colon after "Game "
    if (start == std::string::npos || end == std::string::npos) {
      std::cout << "Couldn't find 'Game ' on line " << line_num << ", skipping: " << line << "\n";
      continue; // No "Game " or colon found, skip the line
    }

    std::string game_id_str = line.substr(start, end - start);
    std::string game_data = line.substr(end + 1);
    std::stringstream roll_ss(game_data);
    std::string roll_item, color_item;

    int32_t game_id;
    if (!absl::SimpleAtoi(game_id_str, &game_id)) {
      std::cout << "Couldn't parse game_id on line " << line_num << ", skipping: " << line << "\n";
      continue; // If conversion fails, skip the line
    }

    bool part_1_possible = true;
    int red_count = 0;
    int green_count = 0;
    int blue_count = 0;

    // each roll
    while (std::getline(roll_ss, roll_item, ';')) {
      std::cout << "Parsing roll:" << roll_item << "\n";
      std::stringstream color_ss(roll_item);

      // each color
      while (std::getline(color_ss, color_item, ',')) {
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
        part_1_possible = false;
      }
    } // each roll

    if (part_1_possible) {
      part_1_sum += game_id;
    }
    part_2_sum += green_count * blue_count * red_count;
  } // each game

  std::cout << std::endl;
  std::cout << "Part 1 sum: " << part_1_sum << "\n";
  std::cout << "Part 2 sum: " << part_2_sum << "\n";
}
