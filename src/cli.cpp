#include "cli.h"
#include "config.h"
#include <cstdint>
#include <expected>
#include <format>
#include <set>
#include <string>
#include <variant>
#include <vector>

struct ShortSwitch {
    char s;
};

struct LongSwitch {
    std::string s;
};

struct Literal {
    std::string value;
};

using Option = std::variant<ShortSwitch, LongSwitch, Literal>;

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

uint16_t parse_isa(const std::string &isa) {
    uint16_t ret = 0;
    for (size_t i = 0; i < isa.size(); i++) {
        switch (isa[i]) {
            case 'i':
            case 'I':
                ret |= RV32I;
                break;
            case 'm':
            case 'M':
                ret |= RV32M;
                break;
            case 'f':
            case 'F':
                ret |= RV32F;
                break;
            case 'e':
            case 'E':
                ret |= RV32E;
                break;
        }
    }
    return ret;
}

std::vector<Option> lex_argv(int argc, char **argv) {
    std::vector<Option> ret;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '\0') {
            continue;
        }
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                ret.push_back( LongSwitch { argv[i] + 2 } );
            }
            else {
                int sw_index = 1;
                while (argv[i][sw_index] != '\0') {
                    ret.push_back( ShortSwitch { argv[i][sw_index] } );
                    sw_index++;
                }
            }
        }
        else {
            ret.push_back( Literal { argv[i] } );
        }
    }
    return ret;
}

std::expected<std::string, std::string> parse_argv(int argc, char **argv) {
    static const std::set<char> argumented_short = {'d'};
    static const std::set<std::string> argumented_long = {};

    std::vector<Option> options = lex_argv(argc, argv);
    std::expected<std::string, std::string> ret;
    bool filename = false;

    for (size_t i = 0; i < options.size(); i++) {
        std::visit( overloads {
            [&options, &i, &ret](ShortSwitch& s) {
                // Unargumented switches
                switch (s.s) {
                    case 'd':
                        g_config.interactive = true;
                        return;
                }
                // Argumented switches
                if (!argumented_short.contains(s.s)) {
                    ret = std::unexpected(std::format("option -{} not recognized!", s.s));
                    return;
                }
                i++;
                if (i >= options.size() || !std::holds_alternative<Literal>(options[i])) {
                    ret = std::unexpected(std::format("-{} expects an argument!", s.s));
                    return;
                }
                std::string argument = std::get<Literal>(options[i]).value;
                switch (s.s) {

                }
            },
            [&options, &i, &ret](LongSwitch& s) {
                // Unargumented switches
                // Argumented switches
                if (!argumented_long.contains(s.s)) {
                    ret = std::unexpected(std::format("option --{} not recognized!", s.s));
                    return;
                }
                i++;
                if (i >= options.size() || !std::holds_alternative<Literal>(options[i])) {
                    ret = std::unexpected(std::format("--{} expects an argument!", s.s));
                    return;
                }
                std::string argument = std::get<Literal>(options[i]).value;
                if (s.s == "isa") {
                    g_config.isa = parse_isa(argument);
                }
                ret = std::unexpected(std::format("--{} not recognized!", s.s));
                return;
            },
            [&filename, &ret](Literal& s) {
                if (!filename) {
                    ret = s.value;
                    filename = true;
                }
                else {
                    ret = std::unexpected(std::format("Unexpected literal {}", s.value));
                }
            }
        }, options[i] );
        if (!ret.has_value()) {
            return ret;
        }
    }
    if (!filename) {
        return std::unexpected("No filename specified!");
    }
    return ret;
}
