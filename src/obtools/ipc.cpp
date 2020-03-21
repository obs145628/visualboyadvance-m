#include "ipc.h"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>

namespace obs {

IPC::IPC() : _running(false), _fd_in(-1), _fd_out(-1) {}

IPC::~IPC() {
  if (_fd_in >= 0)
    close(_fd_in);
  if (_fd_out >= 0)
    close(_fd_out);
}

void IPC::run() {

  // Create pipes
  // When writing something to fds_xx[1], in can be read from fds_xx[0]
  int fds_in[2];
  int fds_out[2];
  if (pipe(fds_in) < 0 || pipe(fds_out) < 0)
    throw std::runtime_error{"IPC: failed to create pipes"};

  pid_t pid = fork();
  if (pid == -1)
    throw std::runtime_error{"IPC: failed to fork"};

  if (pid > 0) // parent
  {
    _pid = pid;
    _running = true;
    _fd_in = fds_in[0]; // Read from stdout of children
    close(fds_in[1]);
    _fd_out = fds_out[1]; // Write to stdin of children
    close(fds_out[0]);
    return;
  }

  // child
  const char *cmd = _cmd.c_str();
  std::vector<const char *> argv;
  argv.push_back(cmd);
  for (const auto &x : _args)
    argv.push_back(x.c_str());
  argv.push_back(nullptr);

  dup2(fds_out[0], STDIN_FILENO); // Replace stdin by fd parent write to
  dup2(fds_in[1], STDOUT_FILENO); // Replace stdout by fd parent read from
  // Close all fds, not needed
  close(fds_out[0]);
  close(fds_out[1]);
  close(fds_in[0]);
  close(fds_in[1]);

  if (execv(cmd, (char **)&argv[0]) < 0) {
    std::cerr << "Failed to run command: " << strerror(errno) << std::endl;
    std::abort();
  }
}

void IPC::wait() {
  if (waitpid(_pid, &_status, 0) == -1)
    throw std::runtime_error{"IPC: failed to wait for child"};
  _running = false;
}

bool IPC::normal_exit() const { return WIFEXITED(_status); }

int IPC::retcode() const { return WEXITSTATUS(_status); }

size_t IPC::read_bytes(void *out_buf, size_t len) {
  ssize_t res = ::read(_fd_in, out_buf, len);
  if (res < 0) {
    std::cerr << "IPC: Failed to read from stdout process: " << strerror(errno)
              << std::endl;
    std::abort();
  }

  return res;
}

size_t IPC::write_bytes(const void *in_buf, size_t len) {
  ssize_t res = ::write(_fd_out, in_buf, len);
  if (res < 0) {
    std::cerr << "IPC: Failed to write to stdin process: " << strerror(errno)
              << std::endl;
    std::abort();
  }

  return res;
}

std::string IPC::read_until(char sep) {
  std::string res;
  char c;

  while (read_bytes(&c, 1) != 0) {
    res.push_back(c);
    if (c == sep)
      break;
  }

  return res;
}

} // namespace obs
