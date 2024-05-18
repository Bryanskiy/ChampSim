#include "msl/fwcounter.h"
#include "ooo_cpu.h"

namespace
{
constexpr std::size_t GLOBAL_HISTORY_LENGTH = 14;
constexpr std::size_t COUNTER_BITS = 2;
constexpr std::size_t GAP_TABLE_SIZE = 16384; // 2^14
constexpr std::size_t PER_ADDR_PHT_SIZE = 32; // 2^5
constexpr std::size_t MASK = 0x5;

using PHT = std::array<std::array<champsim::msl::fwcounter<COUNTER_BITS>, BIMODAL_TABLE_SIZE>, PER_ADDR_PHT_SIZE>;
std::map<O3_CPU*, PHT> gap_history_table;
std::map<O3_CPU*, std::bitset<GLOBAL_HISTORY_LENGTH>> branch_history_register;

} // namespace

void O3_CPU::initialize_branch_predictor() {}

uint8_t O3_CPU::predict_branch(uint64_t ip)
{
  // load global register
  std::size_t history_register = branch_history_register[this];
  std::size_t history_table_idx = history_register.to_ullong();

  // load pattern from history table
  auto value = ::gag_history_table[this][ip && MASK][history_table_idx];

  // predict
  return value.value() >= (value.maximum / 2);
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  // update history table
  std::size_t history_register = branch_history_register[this];
  std::size_t history_table_idx = history_register.to_ullong();

  ::gap_history_table[this][ip && MASK][history_table_idx] += taken ? 1 : -1;

  // update branch history register
  ::branch_history_register[this] <<= 1;
  ::branch_history_register[this][0] = taken;
}
