#include "elf-module.h"

#include "ipc.h"
#include "strutils.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace obs {

namespace {
TextElfFunction parse_fun_line(const std::string &line) {
  auto parts = str_split(line, ' ');
  assert(parts.size() == 2);
  auto addr = str_trim(parts[0]);
  auto name = str_trim(parts[1]);
  auto addr_dec = std::stol(addr, nullptr, 16);
  name = name.substr(1, name.size() - 3);
  TextElfFunction fun;
  fun.beg_addr = addr_dec;
  fun.symbol = name;

  return fun;
}

std::vector<TextElfIns> parse_code_line(const std::vector<std::string> &parts) {
  std::vector<TextElfIns> res;
  std::string text = parts[2];
  for (auto it = parts.begin() + 3; it != parts.end(); ++it) {
    text.append(" ");
    text.append(str_trim(*it));
  }
  text = str_trim(text);

  auto addr = str_trim(parts[0].substr(0, parts[0].size() - 1));
  auto addr_dec = std::stol(addr, nullptr, 16);

  auto opcode = str_trim(parts[1]);
  auto opcode_list = str_split(opcode, ' ');

  for (const auto &opcode_str : opcode_list) {
    auto opcode_dec = std::stoll(opcode_str, nullptr, 16);
    TextElfIns ins;
    ins.addr = addr_dec;
    ins.opcode = opcode_dec;
    ins.text = text;

    // Guess instruction type
    if (opcode_str.size() == 4)
      ins.type = TextElfIns::Type::THUMB;
    else if (opcode_str.size() == 8)
      ins.type = TextElfIns::Type::ARM;
    else {
      // Haven't seen this before
      assert(0);
    }

    if (ins.type == TextElfIns::Type::THUMB)
      addr_dec += 2;
    else if (ins.type == TextElfIns::Type::ARM)
      addr_dec += 4;

    res.push_back(ins);
  }

  return res;
}
} // namespace

void TextElfIns::dump() const {
  size_t opcode_size = type == Type::ARM ? 8 : 4;
  const char *op_next = opcode_size == 8 ? "" : "    ";
  std::cout << std::hex << std::setfill(' ') << std::setw(8) << addr << ":\t"
            << std::setfill('0') << std::setw(opcode_size) << opcode << op_next
            << "\t" << text << std::dec << std::endl;
  ;
}

void TextElfFunction::init() {
  assert(code.empty() || code.front().addr == beg_addr);

  // Guess type
  type = TextElfIns::Type::UNKNOWN;
  for (const auto &ins : code) {
    if (ins.type == TextElfIns::Type::UNKNOWN) {
      type = TextElfIns::Type::UNKNOWN;
      break;
    }

    else if (type == TextElfIns::Type::UNKNOWN)
      type = ins.type;
    else if (ins.type != type)
      type = TextElfIns::Type::MIXED;
  }

  // Guess end addr
  if (code.empty()) {
    end_addr = beg_addr;
  } else {
    const auto &lins = code.back();
    end_addr = lins.addr;
    if (lins.type == TextElfIns::Type::THUMB)
      end_addr += 2;
    else if (lins.type == TextElfIns::Type::ARM)
      end_addr += 4;
  }
}

void TextElfFunction::dump_header_short() const {
  std::cout << std::hex << std::setfill('0') << std::setw(8) << beg_addr << " <"
            << symbol << ">: " << (code.empty() ? "((empty))\n" : "\n");
}

void TextElfFunction::dump_header_long() const {
  if (type == TextElfIns::Type::ARM)
    std::cout << "[ARMV4] ";
  else if (type == TextElfIns::Type::MIXED)
    std::cout << "[MIXED] ";
  else if (type == TextElfIns::Type::THUMB)
    std::cout << "[THUMB] ";
  else if (type == TextElfIns::Type::UNKNOWN)
    std::cout << "[UNKNW] ";
  std::cout << symbol << " (" << std::hex << beg_addr << " - " << end_addr
            << ")" << std::dec;
  if (code.empty())
    std::cout << "  ((empty))";
  std::cout << std::endl;
}

void TextElfModule::dump_code() const {
  for (const auto &f : funs) {
    f.dump_header_short();
    for (const auto &ins : f.code)
      ins.dump();
    std::cout << std::endl;
  }
}

void TextElfModule::dump_defs() const {
  for (const auto &f : funs)
    f.dump_header_long();
  std::cout << std::endl;
}

void TextElfModule::init() {
  for (auto &f : funs)
    f.init();
}

TextElfModule load_text_elf_module(const std::string &elf_path) {
  IPC ipc;
  ipc.set_cmd("/opt/devkitpro/devkitARM/bin/arm-none-eabi-objdump");
  ipc.set_args({"-S", elf_path});
  ipc.run();

  const std::set<std::string> IGNORE_FUNS = {
      "__boot_method",
      "__slave_number",
  };

  TextElfModule mod;
  TextElfFunction *fun = nullptr;

  for (;;) {
    std::string line = ipc.read_until('\n');
    bool is_eof = line.empty() || line.back() != '\n';
    if (!is_eof && !line.empty())
      line.pop_back();

    line = str_trim(line);

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
      auto parts = str_split(line, '\t');
      if (parts.size() < 3 || !fun)
        continue;

      auto ins_list = parse_code_line(parts);
      assert(ins_list.size());
      if (fun->code.empty() && fun->beg_addr != ins_list.front().addr) {
        std::cerr << "Fun addr doesn't match with it's first ins addr"
                  << std::endl;
        std::abort();
      }
      for (const auto &ins : ins_list)
        fun->code.push_back(ins);
    }

    if (is_eof)
      break;
  }

  ipc.wait();
  mod.init();
  return mod;
}

} // namespace obs
