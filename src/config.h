#include <cstdint>

const uint16_t RV32I = 1 << 0;
const uint16_t RV32M = 1 << 1;
const uint16_t RV32F = 1 << 2;
const uint16_t RV32E = 1 << 3;

static struct {
    uint16_t isa = RV32I;
    bool interactive = false;
} g_config;
