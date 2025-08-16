#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib> // strtod

// 先頭/末尾の空白を削る（C++98用）
static void trim(std::string& s) {
    std::string::size_type b = s.find_first_not_of(" \t\r\n");
    std::string::size_type e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) { s.clear(); return; }
    s = s.substr(b, e - b + 1);
}

static void printError(const std::string& msg){
    std::cerr << "Error: " << msg << '\n';
}

int main(int argc, char** argv) {
    if (argc != 2) { printError("could not open file."); return 1; }

    // 価格DB（課題付属CSVを data.csv と仮定）
    std::ifstream db("data.csv");
    if (!db) { printError("could not open data.csv"); return 1; }

    BitcoinExchange ex;
    try { ex.loadRatesCSV(db); }
    catch(const std::exception& e){ printError(e.what()); return 1; }

    std::ifstream in(argv[1]);
    if (!in) { printError("could not open file."); return 1; }

    std::string line;
    // ヘッダ "date | value" がある場合は読み飛ばし
    if (std::getline(in, line)) {
        // 先頭行がヘッダっぽくなければ処理に回すために戻す
        if (line.find('|') == std::string::npos || line.find("date") == std::string::npos)
            in.seekg(0); // ヘッダじゃなさそうなら先頭に戻す
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::string::size_type bar = line.find('|');
        if (bar == std::string::npos) {
            printError(std::string("bad input => ") + line);
            continue;
        }

        std::string date = line.substr(0, bar);
        std::string val  = line.substr(bar + 1);
        trim(date); trim(val);

        try {
            if (!BitcoinExchange::isValidDate(date)) {
                printError(std::string("bad input => ") + date);
                continue;
            }

            // C++98では std::stod がないので C の strtod を使う
            const char* cstr = val.c_str();
            char* endp = 0;
            double d = std::strtod(cstr, &endp);
            if (endp == cstr) { // 変換失敗
                printError(std::string("bad input => ") + val);
                continue;
            }

            if (!BitcoinExchange::isValidValue(d)) {
                if (d < 0) printError("not a positive number.");
                else       printError("too large a number.");
                continue;
            }

            double rate = ex.getRateForDate(date);
            double out  = d * rate;
            std::cout << date << " => " << d << " = " << out << '\n';
        } catch (...) {
            printError(std::string("bad input => ") + line);
        }
    }
    return 0;
}
