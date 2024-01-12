/*
 * Wazuh shared modules utils
 * Copyright (C) 2015, Wazuh Inc.
 * December 28, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "timeHelper_test.h"
#include "timeHelper.h"
#include <regex>

void TimeUtilsTest::SetUp() {};

void TimeUtilsTest::TearDown() {};

TEST_F(TimeUtilsTest, CheckTimestamp)
{
    const auto currentTimestamp {Utils::getCurrentTimestamp()};
    const auto timestamp {Utils::getTimestamp(std::time(nullptr))};
    EXPECT_FALSE(currentTimestamp.empty());
    EXPECT_FALSE(timestamp.empty());
}

TEST_F(TimeUtilsTest, CheckTimestampValidFormat)
{
    constexpr auto DATE_FORMAT_REGEX_STR {
        "[0-9]{4}/([0-9]|1[0-2]){2}/(([0-9]|1[0-2]){2}) (([0-9]|1[0-2]){2}):(([0-9]|1[0-2]){2}):(([0-9]|1[0-2]){2})"};
    const auto currentTimestamp {Utils::getCurrentTimestamp()};
    const auto timestamp {Utils::getTimestamp(std::time(nullptr))};
    EXPECT_TRUE(std::regex_match(currentTimestamp, std::regex(DATE_FORMAT_REGEX_STR)));
    EXPECT_TRUE(std::regex_match(timestamp, std::regex(DATE_FORMAT_REGEX_STR)));
}

TEST_F(TimeUtilsTest, CheckTimestampInvalidFormat)
{
    constexpr auto DATE_FORMAT_REGEX_STR {
        "[0-9]{4}/([1-9]|1[0-2])/([1-9]|[1-2][0-9]|3[0-1])(2[0-3]|1[0-9]|[0-9]):([0-9]|[1-5][0-9]):([1-5][0-9]|[0-9])"};
    const auto currentTimestamp {Utils::getCurrentTimestamp()};
    const auto timestamp {Utils::getTimestamp(std::time(nullptr))};
    EXPECT_FALSE(std::regex_match(currentTimestamp, std::regex(DATE_FORMAT_REGEX_STR)));
    EXPECT_FALSE(std::regex_match(timestamp, std::regex(DATE_FORMAT_REGEX_STR)));
}

TEST_F(TimeUtilsTest, CheckCompactTimestampValidFormat)
{
    constexpr auto DATE_FORMAT_REGEX_STR {
        "[0-9]{4}/([0-9]|1[0-2]){2}/(([0-9]|1[0-2]){2}) (([0-9]|1[0-2]){2}):(([0-9]|1[0-2]){2}):(([0-9]|1[0-2]){2})"};
    constexpr auto COMPACT_FORMAT_REGEX_STR {
        "[0-9]{4}([0-9]|1[0-2]){2}(([0-9]|1[0-2]){2})(([0-9]|1[0-2]){2})(([0-9]|1[0-2]){2})(([0-9]|1[0-2]){2})"};
    const auto currentTimestamp {Utils::getCurrentTimestamp()};
    const auto timestamp {Utils::getCompactTimestamp(std::time(nullptr))};
    EXPECT_TRUE(std::regex_match(currentTimestamp, std::regex(DATE_FORMAT_REGEX_STR))) << timestamp;
    EXPECT_TRUE(std::regex_match(timestamp, std::regex(COMPACT_FORMAT_REGEX_STR)));
}

TEST_F(TimeUtilsTest, CheckCompactTimestampInvalidFormat)
{
    constexpr auto DATE_FORMAT_REGEX_STR {
        "[0-9]{4}/([1-9]|1[0-2])/([1-9]|[1-2][0-9]|3[0-1])(2[0-3]|1[0-9]|[0-9]):([0-9]|[1-5][0-9]):([1-5][0-9]|[0-9])"};
    const auto currentTimestamp {Utils::getCurrentTimestamp()};
    const auto timestamp {Utils::getCompactTimestamp(std::time(nullptr))};
    EXPECT_FALSE(std::regex_match(currentTimestamp, std::regex(DATE_FORMAT_REGEX_STR)));
    EXPECT_FALSE(std::regex_match(timestamp, std::regex(DATE_FORMAT_REGEX_STR)));
}

TEST_F(TimeUtilsTest, TimestampToISO8601)
{
    EXPECT_EQ("2020-12-28T18:00:00.000Z", Utils::timestampToISO8601("2020/12/28 15:00:00"));
    EXPECT_EQ("2020-12-29T00:00:00.000Z", Utils::timestampToISO8601("2020/12/28 21:00:00"));
    EXPECT_EQ("", Utils::timestampToISO8601("21:00:00"));
}

TEST_F(TimeUtilsTest, RawTimestampToISO8601)
{
    EXPECT_EQ("2020-11-13T01:54:25.000Z", Utils::rawTimestampToISO8601("1605232465"));
    EXPECT_EQ("", Utils::rawTimestampToISO8601(""));
    EXPECT_EQ("", Utils::rawTimestampToISO8601("abcdefg"));
}
