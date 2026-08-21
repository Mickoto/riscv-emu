#include "cli.h"
#include "config.h"
#include <expected>
#include <format>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

Config g_config = {
    .isa = RV32I,
    .interactive = false,
};

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
using MaybeError = std::optional<std::string>;

struct OptionInfo {
    int argnum;
    std::function<MaybeError(std::vector<std::string> &)> effect;
};

std::function<MaybeError(std::vector<std::string> &)> d_effect = [](auto &a) { g_config.interactive = true; return std::nullopt; };
MaybeError isa_effect(std::vector<std::string> &args) {
    g_config.isa = 0;
    std::string &isa_string = args.front();
    for (size_t i = 0; i < isa_string.size(); i++) {
        switch (isa_string[i]) {
            case 'i':
            case 'I':
                g_config.isa |= RV32I;
                break;
            case 'm':
            case 'M':
                g_config.isa |= RV32M;
                break;
            case 'f':
            case 'F':
                g_config.isa |= RV32F;
                break;
            case 'e':
            case 'E':
                g_config.isa |= RV32E;
                break;
            default:
                return std::format("unknown isa extension {}!", isa_string[i]);
        }
    }
    return std::nullopt;
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

const std::map<char, OptionInfo> short_switches = {
    { 'd', { .argnum = 0, .effect = d_effect } },
};

const std::map<std::string, OptionInfo> long_switches = {
    { "isa", { .argnum = 1, .effect = isa_effect } },
};

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

std::expected<std::string, std::string> parse_argv(int argc, char **argv) {
    std::vector<Option> options = lex_argv(argc, argv);
    std::expected<std::string, std::string> ret;
    bool filename = false;

    for (size_t i = 0; i < options.size(); i++) {
        std::visit( overloads {
            [&options, &i, &ret](ShortSwitch& s) {
                if (!short_switches.contains(s.s)) {
                    ret = std::unexpected(std::format("option -{} not recognized!", s.s));
                    return;
                }
                int argnum = short_switches.at(s.s).argnum;
                std::vector<std::string> arguments;
                for (int j = 0; j < argnum; j++) {
                    i++;
                    if (i >= options.size() || !std::holds_alternative<Literal>(options[i])) {
                        ret = std::unexpected(std::format("-{} expects {} arguments, given {}!", s.s, argnum, j));
                        return;
                    }
                    arguments.push_back(std::get<Literal>(options[i]).value);
                }
                auto result = short_switches.at(s.s).effect(arguments);
                if (result) {
                    ret = std::unexpected(result.value());
                }
            },
            [&options, &i, &ret](LongSwitch& s) {
                if (!long_switches.contains(s.s)) {
                    ret = std::unexpected(std::format("option --{} not recognized!", s.s));
                    return;
                }
                int argnum = long_switches.at(s.s).argnum;
                std::vector<std::string> arguments;
                for (int j = 0; j < argnum; j++) {
                    i++;
                    if (i >= options.size() || !std::holds_alternative<Literal>(options[i])) {
                        ret = std::unexpected(std::format("--{} expects {} arguments, given {}!", s.s, argnum, j));
                        return;
                    }
                    arguments.push_back(std::get<Literal>(options[i]).value);
                }
                auto &effect = long_switches.at(s.s).effect;
                auto result = effect(arguments);
                if (result) {
                    ret = std::unexpected(result.value());
                }
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
