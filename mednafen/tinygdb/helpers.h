#pragma once
#include <cstring>

// ========== Chrono
inline auto chrono_nanosecond() -> u64
{
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
inline auto chrono_microsecond() -> u64 { return chrono_nanosecond() / 1'000; }
inline auto chrono_millisecond() -> u64 { return chrono_nanosecond() / 1'000'000; }

// ========== String

inline auto string_slice(std::string self, int offset = 0, int length = -1) -> std::string
{
  std::string result;
  if (offset < 0)
    offset = self.size() - abs(offset);
  if (offset >= 0 && offset < self.size())
  {
    if (length < 0)
      length = self.size() - offset;
    if (length >= 0)
    {
      result.resize(length);
      // memory::copy(result.get(), self.data() + offset, length);
      memcpy(result.data(), self.data() + offset, length);
    }
  }
  return result;
}

constexpr inline auto toBinary_(const char *s, u64 sum = 0) -> u64
{
  return (
      *s == '0' || *s == '1' ? toBinary_(s + 1, (sum << 1) | *s - '0') : *s == '\'' ? toBinary_(s + 1, sum)
                                                                                    : sum);
}

constexpr inline auto toOctal_(const char *s, u64 sum = 0) -> u64
{
  return (
      *s >= '0' && *s <= '7' ? toOctal_(s + 1, (sum << 3) | *s - '0') : *s == '\'' ? toOctal_(s + 1, sum)
                                                                                   : sum);
}

constexpr inline auto toDecimal_(const char *s, u64 sum = 0) -> u64
{
  return (
      *s >= '0' && *s <= '9' ? toDecimal_(s + 1, (sum * 10) + *s - '0') : *s == '\'' ? toDecimal_(s + 1, sum)
                                                                                     : sum);
}

constexpr inline auto toHex_(const char *s, u64 sum = 0) -> u64
{
  return (
      *s >= 'A' && *s <= 'F' ? toHex_(s + 1, (sum << 4) | *s - 'A' + 10) : *s >= 'a' && *s <= 'f' ? toHex_(s + 1, (sum << 4) | *s - 'a' + 10)
                                                                       : *s >= '0' && *s <= '9'   ? toHex_(s + 1, (sum << 4) | *s - '0')
                                                                       : *s == '\''               ? toHex_(s + 1, sum)
                                                                                                  : sum);
}

//

constexpr inline auto toBinary(const char *s) -> u64
{
  return (
      *s == '0' && (*(s + 1) == 'B' || *(s + 1) == 'b') ? toBinary_(s + 2) : *s == '%' ? toBinary_(s + 1)
                                                                                       : toBinary_(s));
}

constexpr inline auto toOctal(const char *s) -> u64
{
  return (
      *s == '0' && (*(s + 1) == 'O' || *(s + 1) == 'o') ? toOctal_(s + 2) : toOctal_(s));
}

constexpr inline auto toHex(const char *s) -> u64
{
  return (
      *s == '0' && (*(s + 1) == 'X' || *(s + 1) == 'x') ? toHex_(s + 2) : *s == '$' ? toHex_(s + 1)
                                                                                    : toHex_(s));
}

//

constexpr inline auto toNatural(const char *s) -> u64
{
  return (
      *s == '0' && (*(s + 1) == 'B' || *(s + 1) == 'b') ? toBinary_(s + 2) : *s == '0' && (*(s + 1) == 'O' || *(s + 1) == 'o') ? toOctal_(s + 2)
                                                                         : *s == '0' && (*(s + 1) == 'X' || *(s + 1) == 'x')   ? toHex_(s + 2)
                                                                         : *s == '%'                                           ? toBinary_(s + 1)
                                                                         : *s == '$'                                           ? toHex_(s + 1)
                                                                                                                               : toDecimal_(s));
}

constexpr inline auto toInteger(const char *s) -> s64
{
  return (
      *s == '+' ? +toNatural(s + 1) : *s == '-' ? -toNatural(s + 1)
                                                : toNatural(s));
}

//

inline auto toReal(const char *s) -> f64
{
  return atof(s);
}

inline auto hexByte(char* out, u8 value) -> void {
  out[0] = "0123456789ABCDEF"[value >> 4];
  out[1] = "0123456789ABCDEF"[value & 0xF];
}

inline auto hex(u32 value, u32 size = 2, char fill = '0') -> std::string
{
  std::string hexStr;
  for (int i = size - 1; i >= 0; --i)
  {
    hexStr += "0123456789ABCDEF"[(value >> (i * 4)) & 0xF];
  }
  return hexStr;
}
inline std::vector<std::string> string_split(const std::string &str, char delimiter)
{
  std::vector<std::string> result;
  std::string current;

  for (char ch : str)
  {
    if (ch == delimiter)
    {
      result.push_back(current);
      current.clear();
    }
    else
    {
      current += ch;
    }
  }

  result.push_back(current); // dernière portion
  return result;
}
// ========== Vector
template <typename T>
bool vector_contains(const std::vector<T> &vec, const T &value)
{
  for (const auto &element : vec)
  {
    if (element == value)
    {
      return true;
    }
  }
  return false;
}

// ========== Function

// Reset
template <typename T>
inline auto reset(T &value) -> void
{
  value = T{};
}