#include <iostream>
#include <string>
#include <regex>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>

namespace fs = std::filesystem;

struct Settings {
    std::string term;
    std::string outputFile;
    std::vector<std::string> includePaths;
    std::vector<std::string> excludePatterns;
    bool caseInsensitive = false;
    bool literal = false;
    int threads = 4;
};

class FileSearcher {
public:
    FileSearcher(const Settings& settings) : settings(settings) {
        if (!settings.outputFile.empty()) {
            outStream.open(settings.outputFile);
            useOutputFile = true;
        }
        compileRegex();
    }

    void search() {
        std::vector<std::string> files = collectFiles();

        #pragma omp parallel for num_threads(settings.threads)
        for (int i = 0; i < files.size(); ++i) {
            searchInFile(files[i]);
        }

        if (useOutputFile) outStream.close();
    }

private:
    Settings settings;
    std::regex searchRegex;
    std::ofstream outStream;
    bool useOutputFile = false;

    void compileRegex() {
        std::string pattern = settings.literal
            ? std::regex_replace(settings.term, std::regex(R"([\.\^\$\*\+\?\(\)\[\{\\\|])"), R"(\\$&)")
            : settings.term;

        auto flags = std::regex_constants::ECMAScript;
        if (settings.caseInsensitive)
            flags |= std::regex_constants::icase;

        searchRegex = std::regex(pattern, flags);
    }

    std::vector<std::string> collectFiles() {
        std::vector<std::string> result;
        for (const auto& path : settings.includePaths) {
            if (fs::is_regular_file(path)) {
                if (!isExcluded(path)) result.push_back(path);
            } else if (fs::is_directory(path)) {
                for (auto& p : fs::recursive_directory_iterator(path)) {
                    if (fs::is_regular_file(p.path()) && !isExcluded(p.path())) {
                        result.push_back(p.path().string());
                    }
                }
            }
        }
        return result;
    }

    bool isExcluded(const fs::path& path) {
        for (const auto& pattern : settings.excludePatterns) {
            if (path.filename().string().find(pattern) != std::string::npos)
                return true;
        }
        return false;
    }

    void searchInFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file) return;

        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            if (std::regex_search(line, searchRegex)) {
                std::string output = formatMatch(filePath, lineNumber, line);
                if (useOutputFile) {
                    #pragma omp critical
                    outStream << output << "\n";
                } else {
                    #pragma omp critical
                    std::cout << output << "\n";
                }
            }
        }
    }

    std::string formatMatch(const std::string& file, int lineNumber, const std::string& line) {
        std::string result = file + ":" + std::to_string(lineNumber) + ": ";

        std::string coloredLine = line;
        std::smatch match;
        if (std::regex_search(line, match, searchRegex)) {
            std::string matchText = match.str();
            std::string colorStart = "\033[33m"; // yellow
            std::string colorEnd = "\033[0m";
            size_t pos = line.find(matchText);
            if (pos != std::string::npos) {
                coloredLine = line.substr(0, pos) + colorStart + matchText + colorEnd + line.substr(pos + matchText.length());
            }
        }

        result += coloredLine;
        return result;
    }
};

Settings parseArgs(int argc, char** argv) {
    Settings settings;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-e" && i + 1 < argc) {
            settings.term = argv[++i];
        } else if (arg == "-f" && i + 1 < argc) {
            settings.includePaths.push_back(argv[++i]);
        } else if (arg == "-o" && i + 1 < argc) {
            settings.outputFile = argv[++i];
        } else if (arg == "-t" && i + 1 < argc) {
            settings.threads = std::stoi(argv[++i]);
        } else if (arg == "-i") {
            settings.caseInsensitive = true;
        } else if (arg == "--literal") {
            settings.literal = true;
        } else if (arg == "--exclude" && i + 1 < argc) {
            settings.excludePatterns.push_back(argv[++i]);
        }
    }

    if (settings.term.empty() || settings.includePaths.empty()) {
        std::cerr << "Usage: perg -e <term> -f <path> [-o <output.txt>] [-t <threads>] [-i] [--literal] [--exclude <pattern>]\n";
        std::exit(1);
    }

    return settings;
}

int main(int argc, char** argv) {
    Settings settings = parseArgs(argc, argv);
    FileSearcher searcher(settings);
    searcher.search();
    return 0;
}
