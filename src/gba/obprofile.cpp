#include "obprofile.h"

#include <iostream>

namespace obprof {

void exec_ins(bool is_arm, uint32_t opcode, uint32_t opcode_addr) {
  std::cout << (is_arm ? "  ARM: " : "THUMB: ") << std::hex << opcode << " ("
            << opcode_addr << ")" << std::endl;
}

void cpu_event() {}

} // namespace obprof
