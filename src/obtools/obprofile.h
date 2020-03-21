#pragma once

#include <cstdint>

namespace obprof {

// Called when a GBA rom file is loaded
//
// Called from:
// CPULoadRom (src/gba/GBA.cpp)
void load_rom(const char *rom_path);

// Called every time a THUMB / ARM ins is executed
// @param is_arm - if true ARM ins, if false THUMB ins
// @param the actual raw opcode (16b for THUMB, 32b for ARM)
// @param opcode_addr - The GBA addr of the ins (difers from actual PC)
//
// Called from
// - armExecute (src/gba/GBA-arm.cpp)
// - thumbExecute (src/gba/GBA-thumb.cpp)
void exec_ins(bool is_arm, uint32_t opcode, uint32_t opcode_addr);

// Called when something happens that cause it to go out of the ins exec loop
void cpu_event();
} // namespace obprof
