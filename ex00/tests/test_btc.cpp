#include <gtest/gtest.h>
#include <sstream>
#include "BitcoinExchange.hpp"

// 小さな疑似DB（実際の付属データには 2010-08-20 以降の値がある例が確認できる）:contentReference[oaicite:4]{index=4}
static const char* kCSV =
"date,exchange_rate\n"
"2010-08-20,0.07\n"
"2010-09-01,0.06\n"
"2011-01-10,0.33\n";

TEST(Exchange, LoadAndExactDate) {
    BitcoinExchange ex;
    std::istringstream csv(kCSV);
    ex.loadRatesCSV(csv);
    EXPECT_DOUBLE_EQ(ex.getRateForDate("2010-09-01"), 0.06);
}

TEST(Exchange, PreviousLowerDateFallback) {
    BitcoinExchange ex;
    std::istringstream csv(kCSV);
    ex.loadRatesCSV(csv);
    // 2010-08-25 は DB に無い → 直前の 2010-08-20 を使う（仕様）:contentReference[oaicite:5]{index=5}
    EXPECT_DOUBLE_EQ(ex.getRateForDate("2010-08-25"), 0.07);
}

TEST(Exchange, NoEarlierDateThrows) {
    BitcoinExchange ex;
    std::istringstream csv(kCSV);
    ex.loadRatesCSV(csv);
    // 最初のキーより前の日付は直前がないので例外
    EXPECT_THROW(ex.getRateForDate("2009-01-01"), std::runtime_error);
}

TEST(Validation, DateAndValueRules) {
    EXPECT_TRUE(BitcoinExchange::isValidDate("2011-01-03"));   // ヘッダ例で登場する形式 :contentReference[oaicite:6]{index=6}
    EXPECT_FALSE(BitcoinExchange::isValidDate("2001-42-42"));  // 明確に不正な例 :contentReference[oaicite:7]{index=7}

    EXPECT_TRUE(BitcoinExchange::isValidValue(0.0));
    EXPECT_TRUE(BitcoinExchange::isValidValue(1000.0));
    EXPECT_FALSE(BitcoinExchange::isValidValue(-1.0));         // not a positive number
    EXPECT_FALSE(BitcoinExchange::isValidValue(1000.1));       // too large a number
}
