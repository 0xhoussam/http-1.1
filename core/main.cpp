#include "core/Initialization.hpp"
#include "core/Multiplexer.hpp"
#include "libnet/Net.hpp"
#include "libnet/Terminator.hpp"
#include "libparse/Config.hpp"
#include "libparse/match.hpp"
#include "libparse/utilities.hpp"
#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

void sessionsHandler(libnet::Netenv &net, libparse::Config &config) {
  libnet::Sessions::iterator sessionsBegin = net.readySessions.begin();
  libnet::Sessions::iterator sessionsEnd = net.readySessions.end();

  while (sessionsBegin != sessionsEnd) {
    libnet::Session *session = sessionsBegin->second;

    libhttp::Reader::error readerErr = libhttp::Reader::OK;
    libhttp::Writer::erorr writerError = libhttp::Writer::OK;

    // Calling the reader.
    if (session->isNonBlocking(libnet::Session::SOCK_READ)) {
      readerErr = session->reader.read();
      if (readerErr != libhttp::Reader::OK) {
        session->destroy = true;
        sessionsBegin++;
        continue;
      }
    }

    libhttp::Request *request = session->reader.requests.front();

    if (request->state == libnet::SessionState::READING_BODY ||
        request->state == libnet::SessionState::READING_FIN)
      libhttp::Mux::multiplexer(session, config);

    // Calling the writer.
    if (session->isNonBlocking(libnet::Session::SOCK_WRITE)) {
      writerError = session->writer.write(session->isNonBlocking(libnet::Session::WRITER_READ));
      if (writerError != libhttp::Writer::OK) {
        session->destroy = true;
        sessionsBegin++;
        continue;
      }
    }

    sessionsBegin++;
  };
}

int main(int argc, char *argv[]) {
  libparse::Config config;
  libnet::Netenv   net;

  if (argc < 2) {
    std::cerr << "error: missing config file \n";
    return 0;
  }

  if (libparse::checkConfig(argv[1], config) == false)
    return EXIT_FAILURE;

  if (!config.defaultServer) {
    std::cerr << "missing default server in the config" << std::endl;
    return 1;
  }

  if (WebServ::initializeFsEnv())
    return EXIT_FAILURE;

  net.setupSockets(config);

  while (true) {
    net.prepFdSets();
    net.awaitEvents();

    sessionsHandler(net, config);

    if (net.readySockets.empty() != true)
      net.acceptNewClients();

    libnet::Terminator::terminate(net.sessions);
  };
  return 0;
}
