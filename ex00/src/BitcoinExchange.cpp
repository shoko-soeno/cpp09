#include "BitcoinExchange.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <cstdlib>    // strtod
#include <iomanip>

using std::string;

static bool allDigits(const string& s, size_t expectLen) {
    if (s.size() != expectLen) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(s[i]);
        if (!std::isdigit(ch)) return false;
    }
    return true;
}

bool BitcoinExchange::isValidDate(const string& ymd) {
    // 形式 YYYY-MM-DD を簡易チェック（厳密な月日判定は省略）
    if (ymd.size() != 10 || ymd[4] != '-' || ymd[7] != '-') return false;
    string y = ymd.substr(0, 4);
    string m = ymd.substr(5, 2);
    string d = ymd.substr(8, 2);
    if (!allDigits(y, 4) || !allDigits(m, 2) || !allDigits(d, 2)) return false;

    // 文字列→数値（C++98なので stringstream で）
    std::istringstream ms(m), ds(d);
    int mi = 0, di = 0;
    if (!(ms >> mi) || !(ds >> di)) return false;
    if (mi < 1 || mi > 12 || di < 1 || di > 31) return false;

    return true;
}

bool BitcoinExchange::isValidValue(double v) {
    return v >= 0.0 && v <= 1000.0;
}

void BitcoinExchange::loadRatesCSV(std::istream& csv) {
    string line;
    bool first = true;
    while (std::getline(csv, line)) {
        if (line.empty()) continue;

        // 先頭行がヘッダなら飛ばす
        if (first && line.find("date") != string::npos) {
            first = false;
            continue;
        }
        first = false;

        size_t pos = line.find(',');
        if (pos == string::npos) continue;

        string date = line.substr(0, pos);
        string rateStr = line.substr(pos + 1);

        if (!isValidDate(date)) continue;

        // 文字列→double（C標準の strtod でもOK）
        char* endp = 0;
        const char* c = rateStr.c_str();
        double r = std::strtod(c, &endp);
        if (endp == c) continue; // 変換失敗

        rates_[date] = r;
    }
    if (rates_.empty()) throw std::runtime_error("No rates loaded");
}

double BitcoinExchange::getRateForDate(const string& ymd) const {
    if (!isValidDate(ymd)) throw std::invalid_argument("Invalid date");

    std::map<string, double>::const_iterator it = rates_.lower_bound(ymd);

    if (it == rates_.end()) {
        if (rates_.empty()) throw std::runtime_error("No data");
        // 末尾（最も過去に近い“最後”）を使う
        it = rates_.end();
        --it;
        return it->second;
    }

    if (it->first == ymd) {
        return it->second;
    }

    if (it == rates_.begin()) {
        // 直前が存在しない
        throw std::runtime_error("No earlier date");
    }

    // ひとつ前へ（std::prevはC++11なので使わない）
    --it;
    return it->second;
}
