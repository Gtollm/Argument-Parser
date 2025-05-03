#include <fstream>
#include <gtest/gtest.h>
#include <lib/ArgParser.hpp>
#include <sstream>
#include <string>

using namespace ArgumentParser;

/*
    Функция принимает в качество аргумента строку, разделяет ее по "пробелу"
    и возвращает вектор полученных слов
*/
std::vector<std::string> SplitString(const std::string& str) {
  std::istringstream iss(str);

  return {std::istream_iterator<std::string>(iss),
          std::istream_iterator<std::string>()};
}

TEST(ArgParserTestSuite, EmptyTest) {
  ArgParser parser("My Empty Parser");

  ASSERT_TRUE(parser.Parse(SplitString("app")));
}

TEST(ArgParserTestSuite, StringTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, StringWithSpaceTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1 value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, ShortNameTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument('p', "param1");

  ASSERT_TRUE(parser.Parse(SplitString("app -p=value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, ShortNameWithSpaceTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument('p', "param1");

  ASSERT_TRUE(parser.Parse(SplitString("app -p=value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, DefaultTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1").Default("value1");

  ASSERT_TRUE(parser.Parse(SplitString("app")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, NoDefaultTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1");

  ASSERT_FALSE(parser.Parse(SplitString("app")));
}

TEST(ArgParserTestSuite, StoreValueTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
  ASSERT_EQ(value, "value1");
}

TEST(ArgParserTestSuite, MultiStringTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);
  parser.AddStringArgument('a', "param2");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1=value1 --param2=value2")));
  ASSERT_EQ(parser.GetStringValue("param2"), "value2");
}

TEST(ArgParserTestSuite, MultiStringWithSpaceTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);
  parser.AddStringArgument('a', "param2");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1=value1 --param2 value2")));
  ASSERT_EQ(parser.GetStringValue("param2"), "value2");
}

TEST(ArgParserTestSuite, MultiStringWithSpacesTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);
  parser.AddStringArgument('a', "param2");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1 value1 --param2 value2")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST(ArgParserTestSuite, IntTest) {
  ArgParser parser("My Parser");
  parser.AddIntArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=100500")));
  ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}

TEST(ArgParserTestSuite, IntWithSpaceTest) {
  ArgParser parser("My Parser");
  parser.AddIntArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1 100500")));
  ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}

TEST(ArgParserTestSuite, IntWithSpaceCopyTest) {
  ArgParser parser_test("My Parser");
  parser_test.AddIntArgument("param1");

  ArgParser parser(parser_test);

  ASSERT_TRUE(parser.Parse(SplitString("app --param1 100500")));
  ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}

TEST(ArgParserTestSuite, MultiValueTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  parser.AddIntArgument('p', "param1").MultiValue().StoreValues(int_values);

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
  ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
  ASSERT_EQ(int_values[1], 2);
  ASSERT_EQ(int_values[2], 3);
}

TEST(ArgParserTestSuite, MultiValueWithSpaceTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  parser.AddIntArgument('p', "param1").MultiValue().StoreValues(int_values);

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1=1 --param1 2 --param1=3")));
  ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
  ASSERT_EQ(int_values[1], 2);
  ASSERT_EQ(int_values[2], 3);
}

TEST(ArgParserTestSuite, MultiValueWithSpaceMoveTest) {
  ArgParser parser_temp("My Parser");
  std::vector<int> int_values;
  parser_temp.AddIntArgument('p', "param1")
      .MultiValue()
      .StoreValues(int_values);
  ArgParser parser = std::move(parser_temp);
  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1=1 --param1 2 --param1=3")));
  ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
  ASSERT_EQ(int_values[1], 2);
  ASSERT_EQ(int_values[2], 3);
}

TEST(ArgParserTestSuite, MultiValueWithSpacesTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  parser.AddIntArgument('p', "param1").MultiValue().StoreValues(int_values);

  ASSERT_TRUE(
      parser.Parse(SplitString("app --param1 1 --param1 2 --param1 3")));
  ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
  ASSERT_EQ(int_values[1], 2);
  ASSERT_EQ(int_values[2], 3);
}

TEST(ArgParserTestSuite, MinCountMultiValueWithSpaceTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  size_t MinArgsCount = 10;
  parser.AddIntArgument('p', "param1")
      .MultiValue(MinArgsCount)
      .StoreValues(int_values);

  ASSERT_FALSE(
      parser.Parse(SplitString("app --param1 1 --param1 2 --param1 3")));
}

TEST(ArgParserTestSuite, MinCountMultiValueTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  size_t MinArgsCount = 10;
  parser.AddIntArgument('p', "param1")
      .MultiValue(MinArgsCount)
      .StoreValues(int_values);

  ASSERT_FALSE(
      parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}

TEST(ArgParserTestSuite, FlagTest) {
  ArgParser parser("My Parser");
  parser.AddFlag('f', "flag1");

  ASSERT_TRUE(parser.Parse(SplitString("app --flag1")));
  ASSERT_TRUE(parser.GetFlag("flag1"));
}

TEST(ArgParserTestSuite, FlagsTest) {
  ArgParser parser("My Parser");
  bool flag3;
  parser.AddFlag('a', "flag1");
  parser.AddFlag('b', "flag2").Default(true);
  parser.AddFlag('c', "flag3").StoreValue(flag3);

  ASSERT_TRUE(parser.Parse(SplitString("app -ac")));
  ASSERT_TRUE(parser.GetFlag("flag1"));
  ASSERT_TRUE(parser.GetFlag("flag2"));
  ASSERT_TRUE(flag3);
}

TEST(ArgParserTestSuite, FlagsTest2) {
  ArgParser parser("My Parser");
  bool flag3;
  parser.AddFlag('a', "flag1");
  parser.AddFlag('b', "flag2").Default(false);
  parser.AddFlag('c', "flag3").StoreValue(flag3);

  ASSERT_TRUE(parser.Parse(SplitString("app -a -c")));
  ASSERT_TRUE(parser.GetFlag("flag1"));
  ASSERT_FALSE(parser.GetFlag("flag2"));
  ASSERT_TRUE(flag3);
}

TEST(ArgParserTestSuite, FlagValue) {
  ArgParser parser("My Parser");
  parser.AddStringArgument('a', "flag1");
  parser.AddFlag('b', "flag2");

  ASSERT_TRUE(parser.Parse(SplitString("app -aFlag --flag2")));
  ASSERT_EQ(parser.GetStringValue("flag1"), "Flag");
  ASSERT_TRUE(parser.GetFlag('b'));
}

TEST(ArgParserTestSuite, FlagValueMultiValue) {
  ArgParser parser("My Parser");
  parser.AddIntArgument('a', "flag1").MultiValue();
  parser.AddFlag('b', "flag2").Positional();

  ASSERT_TRUE(parser.Parse(SplitString("app -a1 -a2 -a4 -b")));
  ASSERT_EQ(parser.GetIntValue('a', 1), 2);
  ASSERT_TRUE(parser.GetFlag("flag2"));
}

TEST(ArgParserTestSuite, PositionalArgTest) {
  ArgParser parser("My Parser");
  std::vector<int> values;
  parser.AddIntArgument("Param1").MultiValue(1).Positional().StoreValues(
      values);

  ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5")));
  ASSERT_EQ(values[0], 1);
  ASSERT_EQ(values[2], 3);
  ASSERT_EQ(values.size(), 5);
}

TEST(ArgParserTestSuite, PositionalAndNormalArgTest) {
  ArgParser parser("My Parser");
  std::vector<int> values;
  parser.AddFlag('f', "flag", "Flag");
  parser.AddIntArgument('n', "number", "Some Number");
  parser.AddIntArgument("Param1").MultiValue(1).Positional().StoreValues(
      values);

  ASSERT_TRUE(parser.Parse(SplitString("app -n 0 1 2 3 4 5 -f")));
  ASSERT_TRUE(parser.GetFlag("flag"));
  ASSERT_EQ(parser.GetIntValue("number"), 0);
  ASSERT_EQ(values[0], 1);
  ASSERT_EQ(values[2], 3);
  ASSERT_EQ(values.size(), 5);
}

TEST(ArgParserTestSuite, RepeatedParsingTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file");
  parser.AddStringArgument('o', "output", "File path for output directory");
  parser.AddFlag('s', "flag1", "Read first number");
  parser.AddFlag('p', "flag2", "Read second number");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --number 2 -s -i test -o=test")));

  if (parser.GetFlag("flag1")) {
    parser.AddIntArgument("first", "First Number");
  } else if (parser.GetFlag("flag2")) {
    parser.AddIntArgument("second", "Second Number");
  }

  ASSERT_TRUE(parser.Parse(
      SplitString("app --number 2 -s -i test -o=test --first=52")));
  ASSERT_EQ(parser.GetIntValue("first"), 52);
}

TEST(ArgParserTestSuite, RepeatedParsingReversedTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file");
  parser.AddStringArgument('o', "output", "File path for output directory");
  parser.AddFlag('s', "flag1", "Read first number");
  parser.AddFlag('p', "flag2", "Read second number");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --number=2 -s -i=test -o test")));

  if (parser.GetFlag("flag1")) {
    parser.AddIntArgument("first", "First Number ");
  } else if (parser.GetFlag("flag2")) {
    parser.AddIntArgument("second", "Second Number");
  }

  ASSERT_TRUE(parser.Parse(
      SplitString("app --number 2 -s -i test -o test --first 52")));
  ASSERT_EQ(parser.GetIntValue("first"), 52);
}
TEST(ArgParserTestSuite, RepeatedParsingTest2) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file");
  parser.AddStringArgument('o', "output", "File path for output directory");
  parser.AddFlag('s', "flag1", "Read first number");
  parser.AddFlag('p', "flag2", "Read second number");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_TRUE(
      parser.Parse(SplitString("app --number 2 -s -i test -o=test")));

  if (parser.GetFlag("flag1")) {
    parser.AddIntArgument("first", "First Number");
  } else if (parser.GetFlag("flag2")) {
    parser.AddIntArgument("second", "Second Number");
  }

  ASSERT_TRUE(parser.Parse(
      SplitString("app --number 2 -s -i test -o=test --first=52")));
  ASSERT_EQ(parser.GetIntValue("first"), 52);
}

TEST(ArgParserTestSuite, HelpTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");

  ASSERT_TRUE(parser.Parse(SplitString("app --help")));
  ASSERT_TRUE(parser.Help());
}

TEST(ArgParserTestSuite, HelpStringTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file")
      .MultiValue(1);
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("numer", "Some Number");

  ASSERT_TRUE(parser.Parse(SplitString("app --help")));

  ASSERT_EQ(parser.HelpDescription(),
            "My Parser\nSome Description about program\n\n--numer = <Int>, "
            "Some Number\n-p, --flag2 = <Bool>, Use some logic[ default: 0 "
            "]\n-s, --flag1 = <Bool>, Use some logic[ default: 1 ]\n-i, "
            "--input = <String>, File path for input file[ repeated, "
            "min_args = 1]\n-h, --help, Some Description about program\n");
}
TEST(ArgParserTestSuite, HelpStringPosTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file")
      .MultiValue(1)
      .Positional();
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("numer", "Some Number");

  ASSERT_TRUE(parser.Parse(SplitString("app --help")));

  ASSERT_EQ(
      parser.HelpDescription(),
      "My Parser\nSome Description about program\n\n--numer = <Int>, Some "
      "Number\n-p, --flag2 = <Bool>, Use some logic[ default: 0 ]\n-s, "
      "--flag1 = <Bool>, Use some logic[ default: 1 ]\n-i, --input = "
      "<String>, File path for input file[ positional ][ repeated, min_args "
      "= 1]\n-h, --help, Some Description about program\n");
}

TEST(ArgParserTestSuite, HelpCopyTest) {
  ArgParser parser_test("My Parser");
  parser_test.AddHelp('h', "help", "Some Description about program");
  parser_test.AddStringArgument('i', "input", "File path for input file")
      .MultiValue(1)
      .Positional();
  parser_test.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser_test.AddFlag('p', "flag2", "Use some logic");
  parser_test.AddIntArgument("numer", "Some Number");
  ArgParser parser;
  parser = parser_test;
  ASSERT_TRUE(parser.Parse(SplitString("app --help")));

  ASSERT_EQ(
      parser.HelpDescription(),
      "My Parser\nSome Description about program\n\n--numer = <Int>, Some "
      "Number\n-p, --flag2 = <Bool>, Use some logic[ default: 0 ]\n-s, "
      "--flag1 = <Bool>, Use some logic[ default: 1 ]\n-i, --input = "
      "<String>, File path for input file[ positional ][ repeated, min_args "
      "= 1]\n-h, --help, Some Description about program\n");
}

TEST(ArgParserTestSuite, HelpMoveTest) {
  ArgParser parser_test("My Parser");
  parser_test.AddHelp('h', "help", "Some Description about program");
  parser_test.AddStringArgument('i', "input", "File path for input file")
      .MultiValue(1)
      .Positional();
  parser_test.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser_test.AddFlag('p', "flag2", "Use some logic");
  parser_test.AddIntArgument("numer", "Some Number");
  ArgParser parser = std::move(parser_test);
  ASSERT_TRUE(parser.Parse(SplitString("app --help")));

  ASSERT_EQ(
      parser.HelpDescription(),
      "My Parser\nSome Description about program\n\n--numer = <Int>, Some "
      "Number\n-p, --flag2 = <Bool>, Use some logic[ default: 0 ]\n-s, "
      "--flag1 = <Bool>, Use some logic[ default: 1 ]\n-i, --input = "
      "<String>, File path for input file[ positional ][ repeated, min_args "
      "= 1]\n-h, --help, Some Description about program\n");
}
