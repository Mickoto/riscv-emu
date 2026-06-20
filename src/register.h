#include <cstdint>
#include <cstring>
#include <string>

class RegisterFile {
private:
    uint32_t registers[32];
    uint32_t pc = 0;
    // TODO: CSR

public:
    RegisterFile() {
        memset(registers, 0, sizeof(registers));
    };

    void setRegister(int reg, uint32_t word);
    uint32_t readRegister(int reg) const;
    void printRegisters(std::ostream &os);

    uint32_t getPC() const;
    void setPC(uint32_t value);
    void jumpPC(uint32_t offset);
};

std::string getRegisterName(int reg);
