#include "obprofile.h"

#include <iostream>

namespace obprof {

void load_rom(const char *rom_path) {
  std::cout << "ROM loaded: " << rom_path << std::endl;
}

void exec_ins(bool is_arm, uint32_t opcode, uint32_t opcode_addr) {
  return;
  std::cout << (is_arm ? "  ARM: " : "THUMB: ") << std::hex << opcode << " ("
            << opcode_addr << ")" << std::endl;
}

void cpu_event() {}

} // namespace obprof
