#pragma once

#include <concepts>
#include <sstream>
#include <string>

constexpr char kSpace = ' ';
constexpr char kComma = ',';
constexpr char kNewLine = '\n';
constexpr char kHyphen = '-';
constexpr const char* kDoubleHyphen = "--";

namespace ArgumentParser {

template <typename T>
concept Parsable =
    std::same_as<T, std::string> || requires(const std::string& str, T t) {
      { std::istringstream{str} >> t };
      { std::to_string(t) == str };
      { std::to_string(t) } -> std::same_as<std::string>;
    };

static_assert(Parsable<int>, "int is parsable");
static_assert(Parsable<double>, "double is parsable");
static_assert(Parsable<std::string>, "string is parsable");
static_assert(Parsable<bool>, "bool is parsable");
static_assert(!Parsable<void>, "void is not parsable");

}  // namespace ArgumentParser
