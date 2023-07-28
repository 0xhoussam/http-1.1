#pragma once

#include <map>
#include <string>

namespace libhttp {
  typedef std::map<std::string, std::string> HeadersMap;
  typedef std::pair<std::string, std::string> HeaderPair;

  struct Headers {
    HeadersMap headers;

    void delHeader(std::string);
    std::string &operator[](std::string);
  };
} // namespace libhttp
