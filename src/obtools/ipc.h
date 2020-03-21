//===-- utils/ipc.hh - IPC class definition ---------------------*- C++ -*-===//
//
// OBCL compilers library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definiton on the IPC Class (Inter-Process
/// Communication)
///
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

namespace obs {

/// IPC (Interprocess Communication)
/// Start a new binary process
/// Many features:
/// - Wait for process end
/// - Get returns status
/// - Read / Write bata through stdin/stdout
class IPC {

public:
  IPC();
  ~IPC();

  IPC(const IPC &) = delete;
  IPC(IPC &&) = delete;

  /// et the binary name (relative to the current program)
  void set_cmd(const std::string &cmd) { _cmd = cmd; }

  /// Set the program args (without program name)
  void set_args(const std::vector<std::string> &args) { _args = args; }

  // start running the program, non-blocking
  void run();

  /// wait until the program finished
  void wait();

  /// @returns true if the program exited normally
  /// must be called only after wait
  bool normal_exit() const;

  /// @return the return value of the program
  /// must be called only after wait
  int retcode() const;

  /// Try to read `len` bytes from stdout of process
  /// Store data in `out`
  /// @Returns number of actual bytes read
  /// Wrapper arround read, and error checking
  /// No buffering / Multiple read
  size_t read_bytes(void *out_buf, size_t len);

  /// Try to write `len` bytes to stdin of process
  /// Bytes to be written are in `in_buf`
  /// @Returns number of actual bytes writen
  /// Wrapper around write, and eror checking
  // No buffering / multiple write
  size_t write_bytes(const void *in_buf, size_t len);

  /// Try to read unntil it gets the `sep` char
  /// Sep is at the end of the string, unless it reaches EOFZ
  std::string read_until(char sep);

private:
  bool _running;
  std::string _cmd;
  std::vector<std::string> _args;
  int _status;
  long _pid;
  int _fd_in;
  int _fd_out;
};

} // namespace obs
