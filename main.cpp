#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp>
#include <boost/timer/timer.hpp>
#include <boost/timer/progress_display.hpp>
#include <boost/program_options.hpp>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <unicode/ustream.h>

using json = nlohmann::json;
namespace po = boost::program_options;

// Function to remove spaces except those between alphanumeric characters
icu::UnicodeString removeExcessSpaces(const icu::UnicodeString& text) {
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher pattern(icu::UnicodeString("(^|[^\\p{L}\\p{N}])\\s+|\\s+([^\\p{L}\\p{N}]|$)"), 0, status);
    if (U_FAILURE(status)) {
        // Handle error
        return text;
    }
    icu::UnicodeString result(text);
    pattern.reset(result);
    return pattern.replaceAll("$1$2", status);
}

// Function to split Chinese sentences into a list of sentences with punctuation
std::vector<icu::UnicodeString> splitChineseSentences(const icu::UnicodeString& text) {
    std::vector<icu::UnicodeString> sentences;
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher pattern(icu::UnicodeString("([^。！？「」『』（）()]*[。！？」』])"), 0, status);
    
    if (U_FAILURE(status)) {
        // Handle error
        return sentences;
    }
    
    pattern.reset(text);
    while (pattern.find()) {
        sentences.push_back(pattern.group(status));
    }
    
    return sentences;
}

// Function to process Chinese text by removing spaces and splitting into sentences
std::vector<icu::UnicodeString> zhSentences(const icu::UnicodeString& txt) {
    if (txt.isEmpty()) {
        throw std::invalid_argument("Input text cannot be empty");
    }
    
    icu::UnicodeString cleanedText = removeExcessSpaces(txt);
    return splitChineseSentences(cleanedText);
}

class Trie {
private:
    struct TrieNode {
        std::unordered_map<UChar32, std::unique_ptr<TrieNode>> children;
        bool isEndOfWord;
        TrieNode() : isEndOfWord(false) {}
    };
    
    std::unique_ptr<TrieNode> root;
    size_t wordCount;

public:
    Trie() : root(std::make_unique<TrieNode>()), wordCount(0) {}

    bool insert(const icu::UnicodeString& word) {
        TrieNode* current = root.get();
        bool isNewWord = false;
        for (int32_t i = 0; i < word.length(); ++i) {
            UChar32 c = word.char32At(i);
            auto& child = current->children[c];
            if (!child) {
                child = std::make_unique<TrieNode>();
                isNewWord = true;
            }
            current = child.get();
        }
        if (!current->isEndOfWord) {
            current->isEndOfWord = true;
            isNewWord = true;
            ++wordCount;
        }
        return isNewWord;
    }

    size_t size() const {
        return wordCount;
    }
};

void processJsonlFile(const std::string& inputFilename, const std::string& outputFilename, int num_sents) {
    Trie lines;
    std::ifstream inputFile(inputFilename);
    std::ofstream outputFile(outputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open input file " << inputFilename << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file " << outputFilename << std::endl;
        return;
    }
    
    json j;
    
    boost::timer::progress_display prog(num_sents);
    
    std::string line;
    while (std::getline(inputFile, line) && lines.size() < static_cast<size_t>(num_sents)) {
        j = json::parse(line);
        icu::UnicodeString txt = icu::UnicodeString::fromUTF8(j["text"].get<std::string>());
        txt.findAndReplace("\n", "");
        if (!txt.isEmpty()) {
            for (const auto& sentence : zhSentences(txt)) {
                icu::UnicodeString trimmed_sentence(sentence);
                trimmed_sentence.trim();
                int32_t sentence_len = trimmed_sentence.length();
                
                if (7 <= sentence_len && sentence_len <= 150) {
                    if (lines.insert(trimmed_sentence)) {
                        std::string utf8_sentence;
                        trimmed_sentence.toUTF8String(utf8_sentence);
                        outputFile << json({{"text", utf8_sentence}}).dump() << std::endl;
                        ++prog;
                    }
                }
                
                if (lines.size() >= static_cast<size_t>(num_sents)) {
                    break;
                }
            }
        }
    }
    
    std::cout << "Total unique lines processed: " << lines.size() << std::endl;
}

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile;
    int numSentences;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input,i", po::value<std::string>(&inputFile)->required(), "input file name")
        ("output,o", po::value<std::string>(&outputFile)->required(), "output file name")
        ("sentences,s", po::value<int>(&numSentences)->required(), "number of sentences to process");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    processJsonlFile(inputFile, outputFile, numSentences);
    
    return 0;
}
