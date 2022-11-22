#pragma once

#include <functional>
#include <string>
// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif
namespace AppEngine {

  // from: https://stackoverflow.com/a/57595105
  template <typename T, typename... Rest>
  void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  };

  std::vector<char> readFile(const std::string& filepath);

}  // namespace AppEngine
