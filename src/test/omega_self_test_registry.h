#pragma once

#include <string>
#include <vector>

namespace omega::selftest
{

using TestFunction = bool (*)();

auto Register(const char* name, TestFunction function) -> bool;
auto Run(const std::vector<std::string>& filters) -> bool;

} // namespace omega::selftest

#define OMEGA_SELF_TEST_JOIN_INNER(left, right) left##right
#define OMEGA_SELF_TEST_JOIN(left, right)       OMEGA_SELF_TEST_JOIN_INNER(left, right)
#define OMEGA_REGISTER_SELF_TEST(name, function)                                               \
    namespace                                                                                  \
    {                                                                                          \
                                                                                               \
    [[maybe_unused]] const bool OMEGA_SELF_TEST_JOIN(omegaSelfTestRegistration, __COUNTER__) = \
        omega::selftest::Register(name, function);                                             \
                                                                                               \
    }
