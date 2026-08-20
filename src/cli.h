#include <expected>
#include <string>

std::expected<std::string, std::string> parse_argv(int argc, char **argv);
