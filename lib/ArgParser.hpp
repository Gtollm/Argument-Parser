#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "AbstractArgument.hpp"
#include "ArgParserContext.hpp"
#include "ArgumentBuilder.hpp"
#include "ArgumentHandler.hpp"
#include "ArgumentVisitor.hpp"
#include "Arguments.hpp"
#include "General.hpp"

#define ADD_ARGUMENT_WRAPPERS(T, TYPENAME)                         \
  ArgumentBuilder<T>& Add##TYPENAME##Argument(char short_name) {   \
    return static_cast<ArgumentBuilder<T>&>(                       \
        this->AddArgument<T>(short_name));                         \
  }                                                                \
  ArgumentBuilder<T>& Add##TYPENAME##Argument(                     \
      char short_name, const std::string_view& long_name) {        \
    return static_cast<ArgumentBuilder<T>&>(                       \
        this->AddArgument<T>(short_name, long_name));              \
  }                                                                \
  ArgumentBuilder<T>& Add##TYPENAME##Argument(                     \
      const std::string_view& long_name) {                         \
    return static_cast<ArgumentBuilder<T>&>(                       \
        this->AddArgument<T>(long_name));                          \
  }                                                                \
  ArgumentBuilder<T>& Add##TYPENAME##Argument(                     \
      const std::string_view& long_name,                           \
      const std::string_view& description) {                       \
    return static_cast<ArgumentBuilder<T>&>(                       \
        this->AddArgument<T>(long_name, description));             \
  }                                                                \
  ArgumentBuilder<T>& Add##TYPENAME##Argument(                     \
      char short_name, const std::string_view& long_name,          \
      const std::string_view& description) {                       \
    return static_cast<ArgumentBuilder<T>&>(                       \
        this->AddArgument<T>(short_name, long_name, description)); \
  }                                                                \
  T Get##TYPENAME##Flag(const char short_name) {                   \
    return *(this->GetAnyFlag<T>(short_name));                     \
  }                                                                \
  T Get##TYPENAME##Value(char short_name, std::size_t n = 0) {     \
    return *(this->GetValue<T>(short_name, n));                    \
  }                                                                \
  T Get##TYPENAME##Value(const std::string_view& long_name,        \
                         std::size_t n = 0) {                      \
    return *(this->GetValue<T>(long_name, n));                     \
  }

namespace ArgumentParser {
class ArgumentBase;

class ArgParser {
 private:
  std::optional<std::string> name_;
  UniquenessVisitor unique_visitor_;
  std::shared_ptr<AbstractBuilder> builder_;
  std::shared_ptr<ParserContext> context_;
  std::shared_ptr<ArgumentHandler> parser_chain_;
  std::shared_ptr<Argument<bool>> help_argument_;
  std::vector<std::string> ArgvToVec(int argc, char** argv);
  std::shared_ptr<std::unordered_map<std::string, std::string>>
      argument_types_;

 public:
  ArgParser();
  ArgParser(const ArgParser& parser);
  ArgParser& operator=(const ArgParser& other);
  ArgParser(ArgParser&& other) noexcept;
  ArgParser(const std::string_view& name);
  ArgParser& operator=(ArgParser&& other) noexcept;

  std::shared_ptr<ArgumentHandler> BuildHandlerChain(
      std::shared_ptr<ParserContext> context);

  bool Parse(const std::vector<std::string>& tokens);
  bool Parse(int argc, char** argv);

  void Reset();
  bool Help() const;

  std::string HelpDescription() const;

  template <Parsable T>
  void RegisterCustomType(const std::string& description);

  ArgumentBuilder<bool>& AddFlag(char short_name);
  ArgumentBuilder<bool>& AddFlag(char short_name,
                                 const std::string_view& long_name);
  ArgumentBuilder<bool>& AddFlag(const std::string_view& long_name);
  ArgumentBuilder<bool>& AddFlag(const std::string_view& long_name,
                                 const std::string_view& description);
  ArgumentBuilder<bool>& AddFlag(char short_name,
                                 const std::string_view& long_name,
                                 const std::string_view& description);
  bool GetFlag(char short_name);

  bool GetFlag(const std::string_view& long_name);

  void AddHelp(char short_name, const std::string_view& description);
  void AddHelp(const std::string_view& long_name,
               const std::string_view& description);
  void AddHelp(char short_name, const std::string_view& long_name,
               const std::string_view& description);

  ADD_ARGUMENT_WRAPPERS(int, Int)
  ADD_ARGUMENT_WRAPPERS(std::string, String)
  ADD_ARGUMENT_WRAPPERS(double, Double)
  ADD_ARGUMENT_WRAPPERS(long, Long)
  ADD_ARGUMENT_WRAPPERS(long long, LongLong)
  ADD_ARGUMENT_WRAPPERS(unsigned int, UnsignedInt)
  ADD_ARGUMENT_WRAPPERS(unsigned long, UnsignedLong)
  ADD_ARGUMENT_WRAPPERS(unsigned long long, UnsignedLongLong)
  ADD_ARGUMENT_WRAPPERS(char, Char)
  ADD_ARGUMENT_WRAPPERS(unsigned char, UnsignedChar)

  template <Parsable T>
  ArgumentBuilder<T>& AddArgument(char short_name);
  template <Parsable T>
  ArgumentBuilder<T>& AddArgument(char short_name,
                                  const std::string_view& long_name);
  template <Parsable T>
  ArgumentBuilder<T>& AddArgument(const std::string_view& long_name);
  template <Parsable T>
  ArgumentBuilder<T>& AddArgument(const std::string_view& long_name,
                                  const std::string_view& description);
  template <Parsable T>
  ArgumentBuilder<T>& AddArgument(char short_name,
                                  const std::string_view& long_name,
                                  const std::string_view& description);
  template <Parsable T>
  std::optional<T> GetAnyFlag(char short_name);

  template <Parsable T>
  std::optional<T> GetValue(char short_name, std::size_t n = 0);
  template <Parsable T>
  std::optional<T> GetValue(const std::string_view& long_name,
                            std::size_t n = 0);
};

}  // namespace ArgumentParser

#include "ArgParser.tpp"
