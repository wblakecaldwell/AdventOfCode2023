#include <iostream>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/status/statusor.h"
#include "absl/container/flat_hash_set.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <memory>

ABSL_FLAG(std::string, input_file, "", "Input file to process");

// Represents a position in a tree of all remaining possible matches for a given
// attempt. For example, if the last two characters are "on", then we'd have an
// instance of this WordTree that's got one more possibilty left - if the next
// letter is "e", then we've matched 1.
// If the previous letter was "t", then this WordTree state has two possible
// matches remaining: (t)wo, (t)hree.
class WordTree;

// Builds a WordTree for searching for words from the left.
std::unique_ptr<WordTree> BuildWordTree();

// Builds a WordTree for searching for words from the right.
std::unique_ptr<WordTree> BuildReverseWordTree();

// Finds the first digit from either the left or right, given the input WordTree.
absl::StatusOr<int> FindFirstDigit(const std::string& line, bool from_left, std::unique_ptr<WordTree>& dictionary);

// Builds two WordTrees, one for each direction we're searching:
// - Forward: index words "one", "two", "three", etc
// - Backward: index words "eno", "owt", "eerht", etc
// Read each line from the input file, traverse from the left with the
// forward index, and from the right with the backward index, keeping
// a collection of running matches, stopping traversal once we find
// a digit, either as a single digit character, or a digit spelled out.
int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);

    // Build an index for each forward and backward searching.
    std::unique_ptr<WordTree> dictionary = BuildWordTree();
    std::unique_ptr<WordTree> reverse_dictionary = BuildReverseWordTree();

    std::string file_name = absl::GetFlag(FLAGS_input_file);
    std::cout << "Loading: " << file_name << std::endl;
    std::ifstream file(absl::StrCat(file_name));
    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        return 1;
    }
    std::string line;
    int sum = 0;
    while (std::getline(file, line)) {
        absl::StatusOr<int> left = FindFirstDigit(line, true, dictionary);
        absl::StatusOr<int> right = FindFirstDigit(line, false, reverse_dictionary);
        if (!left.ok() || !right.ok()) {
            std::cout << "ERROR\n";
            return 2;
        }
        int this_num = *left * 10 + *right;
        std::cout << this_num << ": " << line << "\n";
        sum += this_num;
    }
    std::cout << "\nSum: " << sum << "\n\n";
    return 0;
}

class WordTree {
public:
    WordTree()
        : digit_(-1)
    {
        for (int i = 0; i < 26; i++) {
            next_[i] = nullptr;
        }
    }

    ~WordTree() {
        for (int i = 0; i < 26; i++) {
            delete(next_[i]);
            next_[i] = nullptr;
        }
    }

    // Returns the digit found, or -1 if nothing yet.
    int DigitFound() const {
        return digit_;
    }

    // Finds the WordTree at the next letter, or nullptr if end of line.
    WordTree* Find(char letter) const {
        return next_[Normalize(letter)];
    }

    // Indexes a string into the tree as a possible match.
    void IndexString(std::string str, int digit) {
        if (str.size() == 0) {
            std::cout << "ERROR! Size is zero\n";
            exit(-1);
        }
        // ensure target home
        int letter_index = Normalize(str.at(0));
        if (next_[letter_index] == nullptr) {
            next_[letter_index] = new WordTree();
        }
        // store digit if terminus
        if (str.size() == 1) {
            next_[letter_index]->digit_ = digit;
        } else {
            // rest of string
            next_[letter_index]->IndexString(str.substr(1), digit);
        }
    }

private:
    // Normalizes the letter for our 26-position index, moving 'a' to index 0,
    // 'z' to index 25.
    static int Normalize(char letter) {
        int ret = (int)(letter - 'a');
        if (ret >= 26) {
            std::cout << "Found invalid letter: '" << letter << "'\n";
            exit(-2);
        }
        return ret;
    }

    // if set to something other than -1, we've found a spelled-out digit.
    int digit_ = -1;

    // child nodes of remaining possibile matches, where 'a' is the 0-index,
    // 'z' is the 25th.
    WordTree* next_[26];
};

absl::StatusOr<int> FindFirstDigit(const std::string& line, bool from_left, std::unique_ptr<WordTree>& dictionary) {
    absl::flat_hash_set<WordTree*> possible_matches;

    int step = from_left ? 1 : -1;
    for (int pos = from_left ? 0 : line.size() - 1; pos >= 0 && pos < line.size(); pos += step) {
        // check for digit
        // Note: could have indexed these too, but this is simpler, and keeps the index smaller.
        int digit = line.at(pos) - '0';
        if (digit >= 0 && digit <= 9) {
            return digit;
        }

        // we're spelling - evaluate matches we're in the middle of
        absl::flat_hash_set<WordTree*> new_matches;
        char this_letter = line.at(pos);
        for (auto it = possible_matches.begin(); it != possible_matches.end(); it++) {
            WordTree* found = (*it)->Find(this_letter);
            if (found != nullptr) {
                // match is ongoing - see if finished
                if (int digit_found = found->DigitFound(); digit_found != -1) {
                    // found match!
                    return digit_found;
                }
                new_matches.insert(found);
            }
        }

        // start a new search
        WordTree* next_possible_match = dictionary->Find(this_letter);
        if (next_possible_match != nullptr) {
            new_matches.insert(next_possible_match);
        }
        possible_matches = new_matches;
    }
    return absl::InvalidArgumentError("Couldn't find any digit");
}

std::unique_ptr<WordTree> BuildWordTree() {
    std::unique_ptr<WordTree> ret = std::make_unique<WordTree>();
    ret->IndexString("one", 1);
    ret->IndexString("two", 2);
    ret->IndexString("three", 3);
    ret->IndexString("four", 4);
    ret->IndexString("five", 5);
    ret->IndexString("six", 6);
    ret->IndexString("seven", 7);
    ret->IndexString("eight", 8);
    ret->IndexString("nine", 9);
    ret->IndexString("zero", 0);
    return ret;
}

std::unique_ptr<WordTree> BuildReverseWordTree() {
    std::unique_ptr<WordTree> ret = std::make_unique<WordTree>();
    ret->IndexString("eno", 1);
    ret->IndexString("owt", 2);
    ret->IndexString("eerht", 3);
    ret->IndexString("ruof", 4);
    ret->IndexString("evif", 5);
    ret->IndexString("xis", 6);
    ret->IndexString("neves", 7);
    ret->IndexString("thgie", 8);
    ret->IndexString("enin", 9);
    ret->IndexString("orez", 0);
    return ret;
}

