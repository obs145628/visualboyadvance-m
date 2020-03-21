#include "elf-module.h"

#include "ipc.h"
#include "strutils.h"
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace obs {

namespace {
obs::ElfFunction parse_fun_line(const std::string &line) {
  auto parts = obs::str_split(line, ' ');
  assert(parts.size() == 2);
  auto addr = obs::str_trim(parts[0]);
  auto name = obs::str_trim(parts[1]);
  auto addr_dec = std::stol(addr, nullptr, 16);
  name = name.substr(1, name.size() - 3);
  obs::ElfFunction fun;
  fun.addr = addr_dec;
  fun.symbol = name;
  return fun;
}

obs::ElfIns parse_code_line(const std::vector<std::string> &parts) {
  std::string text = parts[2];
  for (auto it = parts.begin() + 3; it != parts.end(); ++it) {
    text.append(" ");
    text.append(obs::str_trim(*it));
  }
  text = obs::str_trim(text);

  auto addr = obs::str_trim(parts[0].substr(0, parts[0].size() - 1));
  auto addr_dec = std::stol(addr, nullptr, 16);
  auto opcode = obs::str_trim(parts[1]);
  auto opcode_dec = std::stol(opcode, nullptr, 16);
  obs::ElfIns ins;
  ins.addr = addr_dec;
  ins.opcode = opcode_dec;
  ins.text = text;
  return ins;
}
} // namespace

void ElfIns::dump() const {
  std::cout << std::hex << "(" << addr << "): `" << text << "` [" << opcode
            << "]\n"
            << std::dec;
}

void ElfFunction::dump() const {
  std::cout << symbol << ": " << (code.empty() ? "((empty))\n" : "\n");
  for (const auto &ins : code)
    ins.dump();
}

void ElfModule::dump() const {
  for (const auto &f : funs) {
    f.dump();
    std::cout << std::endl;
  }
}

ElfModule load_elf_module(const std::string &elf_path) {
  obs::IPC ipc;
  ipc.set_cmd("/opt/devkitpro/devkitARM/bin/arm-none-eabi-objdump");
  ipc.set_args({"-S", elf_path});
  ipc.run();

  const std::set<std::string> IGNORE_FUNS = {
      "__boot_method",
      "__slave_number",
  };

  obs::ElfModule mod;
  obs::ElfFunction *fun = nullptr;

  for (;;) {
    std::string line = ipc.read_until('\n');
    bool is_eof = line.empty() || line.back() != '\n';
    if (!is_eof && !line.empty())
      line.pop_back();

    line = obs::str_trim(line);

    if (line.find('<') != std::string::npos &&
        line.find(">:") != std::string::npos) {
      auto new_fun = parse_fun_line(line);
      if (IGNORE_FUNS.find(new_fun.symbol) == IGNORE_FUNS.end()) {
        mod.funs.push_back(new_fun);
        fun = &mod.funs.back();
      } else
        fun = nullptr;
    }

    if (line.find(':') != std::string::npos) {
      auto parts = obs::str_split(line, '\t');
      if (parts.size() < 3 || !fun)
        continue;

      auto ins = parse_code_line(parts);
      if (fun->code.empty() && fun->addr != ins.addr) {
        std::cerr << "Fun addr doesn't match with it's first ins addr"
                  << std::endl;
        std::abort();
      }
      fun->code.push_back(ins);
    }

    if (is_eof)
      break;
  }

  ipc.wait();
  return mod;
}

} // namespace obs
