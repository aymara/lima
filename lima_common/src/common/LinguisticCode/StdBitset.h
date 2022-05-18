// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_COMMON_STDBITSET
#define LIMA_COMMON_STDBITSET

#include <bitset>
#include <deque>
#include <ostream>
#include <istream>

#ifdef WIN32
#include <cctype>
#endif

namespace Lima
{

#define LC_WIDTH 128
class LinguisticCode
{
  std::bitset<LC_WIDTH> base;

  static std::hash<std::bitset<LC_WIDTH>> hash_fn;
  LIMA_COMMON_EXPORT static const char alphabet[];

  template <size_t N> // N - bits per character
  inline std::string toString_() const
  {
    char chars[LC_WIDTH / N + 2];
    char *p = chars + (LC_WIDTH / N + 1);
    *p = 0;
    LinguisticCode n = *this;

    do
    {
      p--;
      unsigned char c = n.lsb() & ((1 << N) - 1);
      *p = alphabet[c];
      n >>= N;
    } while (n.toBool());

    return std::string(p);
  }

  inline std::size_t hash() const
  {
    return hash_fn(base);
  }

  LinguisticCode(uint64_t val)
    : base((unsigned long long)(val))
  {
  }

public:

  LinguisticCode() noexcept
    : base(0)
  {
  }

  inline static LinguisticCode fromUInt(uint64_t v)
  {
    return LinguisticCode(v);
  }

  inline static LinguisticCode fromOctString(const std::string& str)
  {
    const char *p = str.c_str();
    const char *e = p + str.size();
    LinguisticCode lc;

    while (p != e)
    {
      char c = *p - 0x30;
      lc <<= 3;
      lc |= c;
      p++;
    }

    return lc;
  }

  LIMA_COMMON_EXPORT static LinguisticCode fromDecString(const std::string& str);

  inline static LinguisticCode fromString(const std::string& str)
  {
    return LinguisticCode::fromDecString(str);
  }

  static std::size_t size() noexcept
  {
    return LC_WIDTH;
  }

  inline bool toBool() const noexcept
  {
    return base.any();
  }

  inline bool operator!() const noexcept
  {
    return ! toBool();
  }

  inline LinguisticCode operator<<(std::size_t pos) const noexcept
  {
    LinguisticCode x = *this;
    x <<= pos;
    return x;
  }

  inline LinguisticCode operator<<=(std::size_t pos) noexcept
  {
    base <<= pos;
    return *this;
  }

  inline LinguisticCode operator>>(std::size_t pos) const noexcept
  {
    LinguisticCode x = *this;
    x >>= pos;
    return x;
  }

  inline LinguisticCode operator>>=(std::size_t pos) noexcept
  {
    base >>= pos;
    return *this;
  }

  inline std::string toHexString() const
  {
    return toString_<4>();
  }

  inline std::string toOctString() const
  {
    return toString_<3>();
  }

  inline std::string toBinString() const
  {
    return toString_<1>();
  }

  LIMA_COMMON_EXPORT std::string toDecString() const;

  inline std::string toString() const
  {
    return toDecString();
  }

  inline unsigned char lsb()
  {
    unsigned char c = 0;
    std::size_t p = 0;
    while (p < 8)
    {
      if (base.test(p))
      {
        c |= ( 0x1 << p );
      }
      p++;
    }

    return c;
  }

  inline static std::ostream& encodeToBinary(std::ostream& os, const LinguisticCode& value)
  {
    std::deque<char> chars;
    LinguisticCode n = value;

    do
    {
      unsigned c = (n.lsb() & 0x7F) << 1;
      chars.push_back(c);
      n >>= 7;
    } while (n.toBool());

    for (std::deque<char>::size_type i=chars.size()-1;i>0;i--)
    {
      chars[i] |= 0x1;
      os.put(chars[i]);
      os.flush();
    }
    os.put(chars[0]);
    os.flush();

    return os;
  }

  inline static LinguisticCode decodeFromBinary(std::istream& is)
  {
    char c = 0x1;
    LinguisticCode tmp(0);

    while((c & 0x1) && is.good())
    {
      is.read((char*)&c, sizeof(char));
      tmp <<= 7;
      tmp |= LinguisticCode((c >> 1) & 0x7F);
    }

    return tmp;
  }

  inline static LinguisticCode decodeFromBinary(unsigned char* &p)
  {
    char c = 0x1;
    LinguisticCode tmp(0);

    while(c & 0x1)
    {
      c = *p; p++;
      tmp <<= 7;
      tmp |= LinguisticCode((c >> 1) & 0x7F);
    }

    return tmp;
  }

  inline LinguisticCode operator~() const noexcept
  {
    LinguisticCode x = *this;
    x.base.flip();
    return x;
  }

  inline LinguisticCode operator&=(const LinguisticCode& other) noexcept
  {
    base &= other.base;
    return *this;
  }

  inline LinguisticCode operator|=(const LinguisticCode& other) noexcept
  {
    base |= other.base;
    return *this;
  }

  inline LinguisticCode operator^=(const LinguisticCode& other) noexcept
  {
    base ^= other.base;
    return *this;
  }

  inline bool operator==(const LinguisticCode& other) const noexcept
  {
    return base == other.base;
  }

  inline bool operator!=(const LinguisticCode& other) const noexcept
  {
    return base != other.base;
  }

  friend inline LinguisticCode operator&(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept;
  friend inline LinguisticCode operator|(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept;
  friend inline LinguisticCode operator^(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept;
  friend inline bool operator<(const LinguisticCode& lhs, const LinguisticCode& rhs);
  friend inline bool operator>(const LinguisticCode& lhs, const LinguisticCode& rhs);

  LIMA_COMMON_EXPORT static const LinguisticCode ZERO;
};

inline LinguisticCode operator&(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept
{
  LinguisticCode x = lhs;
  return x &= rhs;
}

inline LinguisticCode operator|(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept
{
  LinguisticCode x = lhs;
  return x |= rhs;
}

inline LinguisticCode operator^(const LinguisticCode& lhs, const LinguisticCode& rhs) noexcept
{
  LinguisticCode x = lhs;
  return x ^= rhs;
}

inline bool operator<(const LinguisticCode& lhs, const LinguisticCode& rhs)
{
  return lhs.hash() < rhs.hash();
}

inline bool operator>(const LinguisticCode& lhs, const LinguisticCode& rhs)
{
  return lhs.hash() > rhs.hash();
}

inline std::ostream& operator<<(std::ostream& os, const LinguisticCode& value)
{
  os << value.toString();
  return os;
}

inline std::istream& operator>>(std::istream& is, LinguisticCode& value)
{
  std::string tmp;
  char c = 0;

  while (is.get(c))
  {
    if (!std::isspace(c))
    {
      is.unget();
      break;
    }
  }

  while (is.get(c))
  {
    if (!std::isdigit(c))
    {
      is.unget();
      break;
    }
    tmp.push_back(c);
  }

  value = LinguisticCode::fromString(tmp);
  return is;
}

LIMA_COMMON_EXPORT QDebug& operator<<(QDebug& os, const LinguisticCode& value);
LIMA_COMMON_EXPORT QTextStream& operator<<(QTextStream& os, const LinguisticCode& value);

}

#define L_NONE Lima::LinguisticCode::ZERO
#define NONE_1 Lima::LinguisticCode::ZERO

#endif
