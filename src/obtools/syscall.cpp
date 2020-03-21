// Whole file added for syscall

#include "syscall.h"
#include "../gba/GBA.h"
#include "../gba/GBAinline.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ENV_STDIN ("VBAM_SYSCALL_STDIN")
#define ENV_STDOUT ("VBAM_SYSCALL_STDOUT")
#define ENV_STDERR ("VBAM_SYSCALL_STDERR")

#define SC_BUFF_SIZE 2048
#define SC_ERR ((uint32_t)-1)
static char sc_buff[SC_BUFF_SIZE];
static int stdin_fd;
static int stdout_fd;
static int stderr_fd;

static void init_syscalls() {
  char *stdin = getenv(ENV_STDIN);
  if (!stdin || !strcmp(stdin, "-"))
    stdin_fd = 0;
  else {
    stdin_fd = open(stdin, 0, 0666);
  }

  char *stdout = getenv(ENV_STDOUT);
  if (!stdout || !strcmp(stdout, "-"))
    stdout_fd = 1;
  else {
    stdout_fd = open(stdout, 0101, 0666);
  }

  char *stderr = getenv(ENV_STDERR);
  if (!stderr || !strcmp(stderr, "-"))
    stderr_fd = 2;
  else {
    stderr_fd = open(stderr, 0101, 0666);
  }
}

static int get_true_fd(int fd) {
  if (fd == 0)
    return stdin_fd;
  else if (fd == 1)
    return stdout_fd;
  else if (fd == 2)
    return stderr_fd;
  else
    return fd;
}

static void syscall_exit(uint32_t code) { _exit((int)code); }

static uint32_t syscall_read(uint32_t in_fd, uint32_t out_buf, uint32_t count) {
  in_fd = get_true_fd(in_fd);
  if (count > SC_BUFF_SIZE)
    count = SC_BUFF_SIZE;

  ssize_t ret = read(in_fd, sc_buff, count);
  if (ret < 0)
    return SC_ERR;

  for (uint32_t i = 0; i < count; ++i)
    CPUWriteByte(out_buf + i, sc_buff[i]);
  return (uint32_t)ret;
}

static uint32_t syscall_write(uint32_t out_fd, uint32_t in_buf,
                              uint32_t count) {
  out_fd = get_true_fd(out_fd);
  if (count > SC_BUFF_SIZE)
    count = SC_BUFF_SIZE;

  for (uint32_t i = 0; i < count; ++i)
    sc_buff[i] = CPUReadByteQuick(in_buf + i);

  ssize_t ret = write(out_fd, sc_buff, count);
  return ret < 0 ? SC_ERR : (uint32_t)ret;
}

static uint32_t syscall_open(uint32_t path, uint32_t flags, uint32_t mode) {
  for (uint32_t i = 0;; ++i) {
    sc_buff[i] = CPUReadByteQuick(path + i);
    if (!sc_buff[i])
      break;
  }

  int ret = open(sc_buff, flags, mode);
  return ret < 0 ? SC_ERR : (uint32_t)ret;
}

static uint32_t syscall_close(uint32_t fd) {
  fd = get_true_fd(fd);
  int ret = close(fd);
  return ret < 0 ? SC_ERR : 0;
}

void run_syscall() {
  static bool is_init = false;
  if (!is_init) {
    init_syscalls();
    is_init = true;
  }

  uint32_t a0 = CPUReadMemoryQuick(SYSCALL_ARGS_ADDR + 0);
  uint32_t a1 = CPUReadMemoryQuick(SYSCALL_ARGS_ADDR + 4);
  uint32_t a2 = CPUReadMemoryQuick(SYSCALL_ARGS_ADDR + 8);
  uint32_t a3 = CPUReadMemoryQuick(SYSCALL_ARGS_ADDR + 12);
  uint32_t ret = 0;

  // printf("Syscall: %u, %u, %u, %u\n", a0, a1, a2, a3);

  switch (a0) {
  case 1:
    syscall_exit(a1);
    break;

  case 3:
    ret = syscall_read(a1, a2, a3);
    break;

  case 4:
    ret = syscall_write(a1, a2, a3);
    break;

  case 5:
    ret = syscall_open(a1, a2, a3);
    break;

  case 6:
    ret = syscall_close(a1);
    break;

  default:
    fprintf(stderr, "Unknown syscall number: %u. Aborting !", 4);
    _exit(1);
  }

  CPUWriteMemory(SYSCALL_ARGS_ADDR, ret);
}
