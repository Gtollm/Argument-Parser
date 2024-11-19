#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

class ParserContext {
 public:
  ParserContext();
  ~ParserContext() = default;
  void AddToLong(const std::shared_ptr<ArgumentBase>& arg);
  void AddToShort(const std::shared_ptr<ArgumentBase>& arg);
  void AddToPos(const std::shared_ptr<ArgumentBase>& arg);

  inline const std::unordered_map<std::string,
                                  std::shared_ptr<ArgumentBase>>&
  GetLongArguments() const;
  inline const std::unordered_map<char, std::shared_ptr<ArgumentBase>>&
  GetShortArguments() const;
  inline const std::vector<std::shared_ptr<ArgumentBase>>& GetPosArguments()
      const;
  inline std::size_t GetPosIndex() const;
  inline void IncrementPosIndex();

 private:
  std::unordered_map<std::string, std::shared_ptr<ArgumentBase>> long_args_;
  std::unordered_map<char, std::shared_ptr<ArgumentBase>> short_args_;
  std::vector<std::shared_ptr<ArgumentBase>> pos_args_;
  std::size_t pos_index_;
};

}  // namespace ArgumentParser

#include "ArgParserContext.tpp"
