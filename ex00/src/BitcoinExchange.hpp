#pragma once
#include <map>
#include <string>
#include <istream>

class BitcoinExchange {
public:
    // CSV を読み込んで map<YYYY-MM-DD, rate> を構築
    void loadRatesCSV(std::istream& csv);

    // YYYY-MM-DD に対する「その日 or 直前日」のレートを取得（見つからなければ例外）
    double getRateForDate(const std::string& ymd) const;

    // 入力値の検証（0 <= value <= 1000）
    static bool isValidValue(double v);

    // 日付フォーマットと妥当性（簡易チェック）
    static bool isValidDate(const std::string& ymd);

private:
    std::map<std::string, double> rates_; // 辞書順の性質で日付の昇順になる
};
