#pragma once

#include "../libhttp/Request.hpp"
#include "../libhttp/Response.hpp"
#include "../libparse/Config.hpp"
#include "libhttp/Error-generate.hpp"
#include <fcntl.h>
#include <unistd.h>

namespace Webserv {
  struct logger {
    enum error { OK, DUP, OPEN };
    static const char *LOGGERLINE;
    static const char *IPV4;
    static const char *METHOD;
    static const char *VERSIONHTTP;
    static const char *PATH;
    static void        loggerInfo(libhttp::Request &request);
    static void        loggerError(libhttp::Request &request);
    static error       log(libparse::Config &config);
  };
} // namespace Webserv
