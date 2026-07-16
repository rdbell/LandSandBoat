#include "map/command_handler.h"

#include <iostream>

auto runCommandHandler2630SelfTests() -> bool
{
    const auto blank  = commandhandler::detail::ParseCommandLine(" \t", "i");
    const auto parsed = commandhandler::detail::ParseCommandLine("  giveitem 12 bad", "id");
    const auto bulk   = commandhandler::detail::ParseCommandLine("say hello world", "s");
    const auto raw    = commandhandler::detail::ParseCommandLine("echo x", "b");
    const auto ok     = !blank.valid && parsed.valid && parsed.name == "giveitem" && parsed.args.size() == 2 && std::get<int>(parsed.args[0]) == 12 && std::get<double>(parsed.args[1]) == 0.0 && bulk.args.size() == 1 && std::get<std::string>(bulk.args[0]) == "hello world" && std::get<std::string>(raw.args[0]) == "echo x";
    if (!ok)
        std::cerr << "command handler self-test failed\n";
    return ok;
}
