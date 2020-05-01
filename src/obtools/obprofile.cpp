#include "obprofile.h"

#include <iostream>
#include <memory>
#include <odb/server/server-app.hh>

#include "elf-module.h"
#include "vm-api.h"

namespace obprof {

ProfInfos g_prof_infos;

namespace {

std::unique_ptr<odb::ServerApp> g_db = nullptr;
}

void load_rom(const char *rom_path) { g_prof_infos.rom_path = rom_path; }

void exec_ins(bool is_arm, uint32_t opcode, uint32_t opcode_addr) {
  g_prof_infos.next_is_arm = is_arm;
  g_prof_infos.next_opcode = opcode;
  g_prof_infos.next_opcode_addr = opcode_addr;

  if (g_db.get() == nullptr) {
    g_db = std::unique_ptr<odb::ServerApp>(new odb::ServerApp(
        []() { return std::unique_ptr<obs::VMApi>(new obs::VMApi); }));
  }

  g_db->loop();

#if 0
  std::cout << (is_arm ? "  ARM: " : "THUMB: ") << std::hex << opcode << " ("
            << opcode_addr << ")" << std::endl;
#endif
}

void cpu_event() {}

} // namespace obprof
