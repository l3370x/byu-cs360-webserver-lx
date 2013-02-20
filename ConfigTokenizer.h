#ifndef ConfigTokenizer_h
#define ConfigTokenizer_h

#include <string>
#include <vector>

//
// Tokenizer
//
// Tokenizes a string
//

class ConfigTokenizer {
 public:
      // Constructor and deconstructor
    ConfigTokenizer();
    ~ConfigTokenizer();

      // Parse a string into a vector of tokens separated by spaces (by default)
      // or any other delimiters
    std::vector<std::string>& parse(std::string&, const std::string& = " ");
      // Parse a string into a vector of at most two tokens
    std::vector<std::string>& parseTwo(std::string&, const std::string& = " ");

 private:
    std::vector<std::string> tokens_;
};

#endif
