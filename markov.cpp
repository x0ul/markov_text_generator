
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <libgen.h>
#include <random>

extern "C" {
#include "lexer.h"
}

struct WordState{
    WordState() : word_occurrences(0) {}
    std::map<std::string,int> transitions;
    std::multimap<int,std::string> reverse_transitions;
    int word_occurrences;
};

std::random_device g_rd; // only used once to initialise (seed) engine
std::mt19937 g_rng(g_rd()); // random-number engine used (Mersenne-Twister in this case)
std::unordered_map<std::string,WordState> g_markov_table;

void random_walk(const WordState& root, int depth)
{
    if (depth <= 0)
        return;

    std::uniform_int_distribution<int> uni(0, root.word_occurrences - 1);
    int random_integer = uni(g_rng);

    // Choose a random word weighted by probability
    for (auto it : root.reverse_transitions) {
        if (random_integer < it.first) {

            if (it.second != ".")  {
                std::cout << it.second << ' ';
                random_walk(g_markov_table[it.second], depth - 1);
            }

            break;
        }
        random_integer -= it.first;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "USAGE: " << ::basename(argv[0]) << " corpus word_limit\n";
        exit(1);
    }

    FILE* file;
    file = fopen(argv[1], "r");
    if (!file) {
        std::cout << "Could not open " << argv[1] << '\n';
        exit(2);
    }

    yyin = file;
    size_t max_len = std::atoi(argv[2]);


    // Initialize with a period, the 'root' of all sentences
    std::string word;
    std::string next_word(".");

    while (yylex() != 0) {
        word = next_word;
        next_word = yytext;
        std::transform(next_word.begin(), next_word.end(), next_word.begin(), ::tolower);

        WordState& ws = g_markov_table[word];
        ws.word_occurrences++;

        // string to frequency map
        int next_word_count = ws.transitions[next_word];
        ws.transitions[next_word]++;
        
        // frequency to string map
        auto range = ws.reverse_transitions.equal_range(next_word_count);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == next_word) {
                ws.reverse_transitions.erase(it);
                break; // Important! Iterator invalid now.
            }
        }
        next_word_count++;
        ws.reverse_transitions.emplace(next_word_count, next_word);
    }
    fclose(file);

#if 0 // sweet debug printout
    for (auto word : g_markov_table) {
        std::cout << word.first << ' ' << word.second.word_occurrences << " times" << '\n';
        for (auto transition : word.second.reverse_transitions) {
            std::cout << " -> " << transition.first << ' ' << transition.second
                << ", P = " << (float)transition.first / word.second.word_occurrences << '\n';
        }
    }

    for (auto word : g_markov_table) {
        std::cout << word.first << ' ' << word.second.word_occurrences << " times" << '\n';
        for (auto transition : word.second.transitions) {
            std::cout << " -> " << transition.first << ' ' << transition.second << " times"
                << ", P = " << (float)transition.second / word.second.word_occurrences << '\n';
        }
    }
#endif

    // Start at the root, '.'
    const WordState& root = g_markov_table["."];
    random_walk(root, max_len);
    std::cout << '\n';

    return 0;
}
