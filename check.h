#pragma once

#include <stdexcept>
#include <fmt/format.h>

#define CHECK(expr) do { \
  if (!(expr)) { \
    throw std::runtime_error(fmt::format("CHECK Fail: {}", #expr)); \
  } \
} while (false)

#define FATAL(msg) do { \
  throw std::runtime_error(msg); \
} while (false)
