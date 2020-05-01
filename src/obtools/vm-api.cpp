#include "vm-api.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

#include "obprofile.h"

namespace obs {

namespace {

constexpr odb::vm_reg_t REG_SP = 13;
constexpr odb::vm_reg_t REG_LR = 14;
constexpr odb::vm_reg_t REG_PC = 15;
constexpr odb::vm_reg_t REGS_COUNT = 16;

constexpr const char *reg_names[] = {"r0",  "r1", "r2", "r3", "r4",  "r5",
                                     "r6",  "r7", "r8", "r9", "r10", "r11",
                                     "r12", "sp", "lr", "pc"};

// @TODO add registers flags

constexpr odb::vm_size_t MEM_SIZE = 0x0FFFFFFF;

} // namespace

VMApi::VMApi() {}

odb::VMInfos VMApi::get_vm_infos() {
  _load_rom();
  _update();

  odb::VMInfos infos;
  infos.name = "vbam";
  infos.regs_count = REGS_COUNT;
  infos.regs_general = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, REG_LR};
  infos.regs_program_counter = {REG_PC};
  infos.regs_stack_pointer = {REG_SP};
  infos.regs_flags = {}; //@TODO
  infos.memory_size = MEM_SIZE;
  infos.symbols_count = 0; //@TODO
  infos.pointer_size = 4;
  infos.integer_size = 4;
  infos.use_opcode = true;

  return infos;
}

odb::VMApi::UpdateInfos VMApi::get_update_infos() {
  _load_rom();
  _update();

  odb::VMApi::UpdateInfos infos;
  infos.act_addr = _next_addr;

  // @TODO find a way to figure out if op is call / ret
  // infos.state = odb::VMApi::UpdateState::CALL_SUB;
  // infos.state = odb::VMApi::UpdateState::RET_SUB;

  infos.state = odb::VMApi::UpdateState::OK;

  return infos;
}

void VMApi::get_reg(odb::vm_reg_t idx, odb::RegInfos &infos, bool val_only) {
  if (idx >= REGS_COUNT)
    throw odb::VMApi::Error("invalid register index");

  throw odb::VMApi::Error("get_reg not implemented");
  (void)infos;
  (void)val_only;

#if 0
  if (val_only) {
    auto ptr = _reg_ptr(idx);
    assert(infos.val.size() == REG_SIZE);
    std::memcpy(&infos.val[0], ptr, REG_SIZE);
  }

  else {
    infos.idx = idx;
    infos.name = reg_names[idx];
    if (idx < REG_SP) {
      infos.kind = odb::RegKind::general;
    } else if (idx == REG_SP) {
      infos.kind = odb::RegKind::stack_pointer;
    } else if (idx == REG_PC) {
      infos.kind = odb::RegKind::program_counter;
    } else if (idx == REG_ZF) {
      infos.kind = odb::RegKind::flags;
    }
    infos.size = REG_SIZE;
  }
#endif
}

void VMApi::set_reg(odb::vm_reg_t idx, const std::uint8_t *new_val) {
  if (idx >= REGS_COUNT)
    throw odb::VMApi::Error("invalid register index");

  throw odb::VMApi::Error("get_reg not implemented");
  (void)new_val;

#if 0
  auto ptr = _reg_ptr(idx);
  std::memcpy(ptr, new_val, REG_SIZE);
#endif
}

odb::vm_reg_t VMApi::find_reg_id(const std::string &name) {

  throw odb::VMApi::Error("find_reg_id not implemented");
  (void)name;

#if 0
  if (name.size() == 2 && name[0] == 'r') {
    char c1 = name[1];
    if (c1 < '0' && c1 > '9')
      throw odb::VMApi::Error("Invalid register name");
    return c1 - '0';
  }

  if (name.size() == 3 && name[0] == 'r') {
    char c1 = name[1];
    char c2 = name[2];
    if (c1 < '0' || c1 > '9' || c2 < '0' || c2 > '4')
      throw odb::VMApi::Error("Invalid register name");
    return (c1 - '0') * 10 + (c2 - '0');
  }

  if (name == "sp")
    return REG_SP;
  if (name == "pc")
    return REG_PC;
  if (name == "zf")
    return REG_ZF;

  throw odb::VMApi::Error("Invalid register name");
#endif
}

void VMApi::read_mem(odb::vm_ptr_t addr, odb::vm_size_t size,
                     std::uint8_t *out_buf) {
  if (addr >= MEM_SIZE || addr + size > MEM_SIZE)
    throw odb::VMApi::Error("Memory address out of range");

  throw odb::VMApi::Error("read_mem not implemented");
  (void)out_buf;

#if 0
  for (auto it = addr; it < addr + size; ++it) {
    std::uint8_t val = it < MEM_ROM_SIZE ? _cpu._ram[it] : 0;
    *(out_buf++) = val;
  }
#endif
}

void VMApi::write_mem(odb::vm_ptr_t addr, odb::vm_size_t size,
                      const std::uint8_t *buf) {
  if (addr >= MEM_SIZE || addr + size > MEM_SIZE)
    throw odb::VMApi::Error("Memory address out of range");

  throw odb::VMApi::Error("write_mem not implemented");
  (void)buf;

#if 0
  for (auto it = addr; it < addr + size; ++it) {
    if (it < MEM_ROM_SIZE)
      _cpu._ram[it] = *buf;
    ++buf;
  }
#endif
}

std::vector<odb::vm_sym_t> VMApi::get_symbols(odb::vm_ptr_t addr,
                                              odb::vm_size_t size) {
  if (addr >= MEM_SIZE || addr + size > MEM_SIZE)
    throw odb::VMApi::Error("Memory address out of range");

  return {};

#if 0
  auto end = addr + size;
  addr = std::max(addr, MEM_CODE_START);
  std::vector<odb::vm_sym_t> res;

  for (auto it = addr; it < end; ++it) {
    auto idx = it - MEM_CODE_START;
    if (idx >= _cpu._rom.ins.size())
      break;
    const auto &ins = _cpu._rom.ins[idx];
    if (ins.def_sym != SYM_NONE)
      res.push_back(ins.def_sym);
  }

  return res;
#endif
}

odb::SymbolInfos VMApi::get_symb_infos(odb::vm_sym_t idx) {
  throw odb::VMApi::Error("get_symb_infos not implemented");
  (void)idx;

#if 0
  const auto &rom = _cpu._rom;
  if (idx >= rom.syms.size())
    throw odb::VMApi::Error("Invalid symbol index");

  odb::SymbolInfos res;
  res.idx = idx;
  res.name = rom.syms[idx];
  res.addr = MEM_CODE_START + rom.sym_defs[idx];
  return res;
#endif
}

odb::vm_sym_t VMApi::find_sym_id(const std::string &name) {
  throw odb::VMApi::Error("find_sym_id not implemented");
  (void)name;

#if 0
  const auto &rom = _cpu._rom;
  auto it = rom.smap.find(name);
  if (it == rom.smap.end())
    throw odb::VMApi::Error("Invalid symbol name");
  return it->second;
#endif
}

std::string VMApi::get_code_text(odb::vm_ptr_t addr,
                                 odb::vm_size_t &addr_dist) {
  if (addr >= MEM_SIZE)
    throw odb::VMApi::Error("Memory address out of range");
  addr_dist = 0;

  const TextElfFunction *fun = nullptr;

  for (const auto &it : _rom->funs)
    if (addr >= it.beg_addr && addr < it.end_addr) {
      fun = &it;
      break;
    }

  if (!fun)
    return "???";

  const TextElfIns *ins = nullptr;
  for (std::size_t i = 0; i < fun->code.size(); ++i) {
    const auto &it = fun->code[i];
    if (it.addr == addr) {
      ins = &it;
      int next_addr;
      if (i + 1 < fun->code.size())
        next_addr = fun->code[i + 1].addr;
      else
        next_addr = fun->end_addr;
      addr_dist = next_addr - it.addr;
      break;
    }
  }

  if (!ins)
    return "???";

  return ins->text;
}

void VMApi::_update() {
  _next_arm = obprof::g_prof_infos.next_is_arm;
  _next_opcode = obprof::g_prof_infos.next_opcode;
  _next_addr = obprof::g_prof_infos.next_opcode_addr;
}

void VMApi::_load_rom() {
  if (_rom.get() != nullptr)
    return;

  auto rom_path = obprof::g_prof_infos.rom_path;

  std::string gba_path(rom_path);
  auto dpos = gba_path.rfind('.');
  std::string elf_path = gba_path.substr(0, dpos) + ".elf";
  // std::cout << "ROM loaded: " << elf_path << std::endl;
  _rom = std::unique_ptr<TextElfModule>(
      new TextElfModule(load_text_elf_module(elf_path)));
}

} // namespace obs
