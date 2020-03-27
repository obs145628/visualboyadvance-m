#include "obprofile.h"

#include "elf-module.h"
#include <iostream>

namespace obprof {

void load_rom(const char *rom_path) {
  return;

  std::string gba_path(rom_path);
  auto dpos = gba_path.rfind('.');
  std::string elf_path = gba_path.substr(0, dpos) + ".elf";

  std::cout << "ROM loaded: " << elf_path << std::endl;

  auto mod = obs::load_elf_module(elf_path);
  mod.dump();
}

void exec_ins(bool is_arm, uint32_t opcode, uint32_t opcode_addr) {
  return;
  std::cout << (is_arm ? "  ARM: " : "THUMB: ") << std::hex << opcode << " ("
            << opcode_addr << ")" << std::endl;
}

void cpu_event() {}

} // namespace obprof
