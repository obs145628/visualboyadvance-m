#pragma once

#include <string>
#include <vector>

namespace obs {

struct ElfIns {
  int opcode; // 4-bit Thumb or 8-bit ARM opcode
  int addr;   // Absolute address
  std::string
      text; // Text representation of the instruction (with optional comments)

  void dump() const;
};

struct ElfFunction {
  std::string symbol; // symbol name for function
  int addr;           // Must be the same than the first instruction address
  std::vector<ElfIns> code;

  void dump() const;
};

struct ElfModule {
  std::vector<ElfFunction> funs;

  void dump() const;
};

// Load a module from an Elf file path
ElfModule load_elf_module(const std::string &elf_path);

} // namespace obs
