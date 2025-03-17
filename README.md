# Chinese Text Processor

This project provides a C++ program for processing Chinese text. It reads a `.jsonl` file containing text data, cleans and splits the text into sentences, and writes the processed sentences to an output file. The program ensures that only unique sentences of a specified length range are included in the output.

## Features

- **Text Cleaning**: Removes unnecessary spaces while preserving spaces between alphanumeric characters.
- **Sentence Splitting**: Splits Chinese text into sentences based on punctuation marks.
- **Unique Sentence Filtering**: Ensures that only unique sentences are written to the output.
- **Length Constraints**: Filters sentences based on a configurable length range (default: 7–150 characters).
- **Progress Display**: Displays progress as the program processes the input file.

## Dependencies

The project uses the following libraries:

- [ICU](https://unicode-org.github.io/icu/) for Unicode string manipulation and regular expressions.
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing and serialization.
- [Boost.Program_Options](https://www.boost.org/doc/libs/release/doc/html/program_options.html) for command-line argument parsing.
- [Boost.Timer](https://www.boost.org/doc/libs/release/libs/timer/) for progress display.

## Installation

### Prerequisites

Before building the project, ensure you have the following installed:

1. A C++ compiler supporting C++17 or later (e.g., GCC, Clang).
2. ICU library.
3. Boost libraries.
4. nlohmann/json header-only library.

### Build Instructions

1. Clone this repository:
   ```bash
   git clone 
   cd 
   ```

2. Compile the program using your preferred build system or directly with `g++`:
   ```bash
   g++ -std=c++17 main.cpp -o chinese_text_processor \
       -lboost_program_options -lboost_timer -licui18n -licuuc
   ```

## Usage

Run the program from the command line with the following options:

```bash
./chinese_text_processor --input  --output  --sentences 
```

### Options

- `--input, -i`: Path to the input `.jsonl` file containing text data.
- `--output, -o`: Path to the output `.jsonl` file where processed sentences will be saved.
- `--sentences, -s`: Number of unique sentences to process.

### Example

```bash
./chinese_text_processor --input data/input.jsonl --output data/output.jsonl --sentences 1000
```

This command processes up to 1,000 unique sentences from `data/input.jsonl` and writes them to `data/output.jsonl`.

## Input File Format

The input file should be in `.jsonl` format, where each line is a JSON object containing a `text` field. Example:

```json
{"text": "这是第一句话。"}
{"text": "这是第二句话！"}
{"text": "这是第三句话？"}
```

## Output File Format

The output file will also be in `.jsonl` format, containing cleaned and split sentences. Example:

```json
{"text": "这是第一句话。"}
{"text": "这是第二句话！"}
{"text": "这是第三句话？"}
```

## How It Works

1. The program reads each line of the input file and parses it as JSON.
2. It extracts the `text` field and removes unnecessary spaces using ICU's regex capabilities.
3. It splits the cleaned text into individual sentences based on Chinese punctuation marks (`。`, `！`, `？`, etc.).
4. Sentences are filtered to ensure they are unique and within a specified length range (7–150 characters by default).
5. Unique sentences are written to the output file in `.jsonl` format.

## Error Handling

- If the input or output file cannot be opened, an error message will be displayed.
- If invalid arguments are provided, the program will display a help message with usage instructions.

## License

This project is licensed under the MIT License. See `LICENSE` for details.

## Acknowledgments

This project leverages several powerful libraries, including ICU, Boost, and nlohmann/json, to handle complex tasks such as Unicode processing, command-line argument parsing, and JSON manipulation efficiently.
