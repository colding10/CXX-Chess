#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "consts.hpp"
#include "position.hpp"
#include "searcher.hpp"

int parse(std::string c) {
    int fil = c[0] - 'a';
    int rank = c[1] - 1;

    return A1 + fil - 10 * rank;
}

std::string render(int i) {
    int rank = i - A1 / 10;
    int file = i % 10;

    return (char)(file + 'a') + std::to_string(-rank + 1);
}

void tokenize(std::string const &str, const char delim,
              std::vector<std::string> &out) {
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

int main() {
    std::string in_string;
    const char delim = ' ';

    std::set<Position> hist = {
        Position(INITIAL, 0, {true, true}, {true, true}, 0, 0)};
    Searcher searcher;

    while (true) { // FIXME: problem is with getting words from string, segfaults at arr[10] when no 10 indexg
        std::cin >> in_string;
        in_string.erase(in_string.find_last_not_of(" \n\r\t") + 1);
        std::vector<std::string> args;
        tokenize(in_string, delim, args);


        if (args[0] == "uci") {
            std::cout << "id name " << VERSION << "\n";
            std::cout << "uciok"
                      << "\n";
        } else if (args[0] == "isready") {
            std::cout << "readyok"
                      << "\n";
        } else if (args[0] == "quit") {
            break;
        } else if (args[0] == "position" && args[1] == "startpos") {
            hist = {Position(INITIAL, 0, {true, true}, {true, true}, 0, 0)};

            for (int ply = 0; ply < args.size() - 3; ply++) {
                std::string move = args[ply];

                int i = parse(move.substr(0, 2));
                int j = parse(move.substr(2, 2));

                std::string prom = move.substr(4);
                std::transform(prom.begin(), prom.end(), prom.begin(), ::toupper);

                if (ply % 2 == 1) {
                    i = 119 - i, j = 119 - j;
                }

                hist.insert(const_cast<Position &>(*(hist.rbegin())).move(Move(i, j, prom[0])));
            }
        } else if (args[0] == "go") {
            std::cout << args.size() << std::endl;
            int wtime, btime, winc, binc;
            std::cout << args[2] << args[4];
            wtime = std::stoi(args[2]);
            btime = std::stoi(args[4]);
            winc = std::stoi(args[6]);
            binc = std::stoi(args[8]);

            if (hist.size() % 2 == 0) {
                wtime = btime;
                winc = binc;
            }
            float think = std::min(wtime / 40 + winc, wtime / 2 - 1);
            auto start_time = std::chrono::high_resolution_clock::now();
            int ms_time = think * 0.8;
            auto end_time = start_time + std::chrono::milliseconds(ms_time);

            std::string move_str = "";
            int gamma, score;
            Move move;
            bool flag = false;
            for (int depth = 1; depth < 1000; depth++) {
                if (flag) {
                    break;
                }

                std::vector<std::tuple<int, int, Move>> result_moves = searcher.search(hist, depth);
                for (auto result : result_moves) {
                    std::tie(gamma, score, move) = result;

                    if (score >= gamma) {
                        int i = move.i, j = move.j;
                        if (hist.size() % 2 == 0) {
                            i = 119 - i, j = 119 - j;
                        }
                        move_str = render(i) + render(j) + std::to_string(tolower(move.prom));
                        std::cout << "info depth " << depth << " score cp " << score << " pv " << move_str << "\n";
                    }
                    if (move_str.length() && std::chrono::high_resolution_clock::now() > end_time) {
                        flag = true;
                        break;
                    }
                    std::cout << "bestmove " << (move_str.length() ? move_str : "(none)") << "\n";
                }
            }
        }
    }

    return 0;
}