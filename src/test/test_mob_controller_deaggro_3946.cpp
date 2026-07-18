#include "test_mob_controller_deaggro_3946.h"

#include "map/ai/controllers/mob_controller_deaggro_capacity.h"
#include "map/ai/controllers/mob_controller_detection_capacity.h"

#include <iostream>

auto runMobControllerDeaggro3946SelfTests() -> bool
{
    using mobcontrollerdeaggro::CanPursueByScent;
    using mobcontrollerdeaggro::ShouldDeaggroForHide;
    using mobcontrollerdeaggro::ShouldDeaggroForLock;
    using mobcontrollerdetection::Evaluate;

    const auto base = std::chrono::steady_clock::time_point{};
    const bool detectionOK = Evaluate(false, false, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, true, false, base + std::chrono::seconds(25) - std::chrono::nanoseconds(1), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, false, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).shouldDeaggro &&
                             !Evaluate(false, false, false, true, false, base + std::chrono::seconds(27), base, std::chrono::seconds(3)).shouldDeaggro &&
                             Evaluate(false, false, false, true, false, base + std::chrono::seconds(28), base, std::chrono::seconds(3)).shouldDeaggro &&
                             Evaluate(false, false, false, true, true, base + std::chrono::seconds(25), base, std::chrono::hours(1)).shouldDeaggro &&
                             Evaluate(true, false, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro &&
                             Evaluate(false, true, false, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro &&
                             Evaluate(false, false, true, true, false, base + std::chrono::seconds(25), base, std::chrono::seconds(0)).tapDeaggro;

    const bool scentOK = CanPursueByScent(true, false, true, false, false) &&
                         !CanPursueByScent(false, false, true, false, false) &&
                         !CanPursueByScent(true, true, true, false, false) &&
                         !CanPursueByScent(true, false, false, false, false) &&
                         !CanPursueByScent(true, false, true, true, false) &&
                         !CanPursueByScent(true, false, true, false, true);
    const bool hideOK = ShouldDeaggroForHide(true, true, false, false, false) &&
                        !ShouldDeaggroForHide(false, true, false, false, false) &&
                        !ShouldDeaggroForHide(true, false, false, false, false) &&
                        !ShouldDeaggroForHide(true, true, true, false, false) &&
                        !ShouldDeaggroForHide(true, true, false, true, false) &&
                        !ShouldDeaggroForHide(true, true, false, false, true);
    const bool lockOK = ShouldDeaggroForLock(true, false, true, false, false, false) &&
                        ShouldDeaggroForLock(false, true, false, true, true, false) &&
                        !ShouldDeaggroForLock(true, false, false, false, false, false) &&
                        !ShouldDeaggroForLock(false, true, false, false, true, false) &&
                        !ShouldDeaggroForLock(true, false, true, false, false, true);
    if (!scentOK || !detectionOK || !hideOK || !lockOK)
    {
        std::cerr << "mob controller deaggro 3946 self-test failed\n";
    }
    return scentOK && detectionOK && hideOK && lockOK;
}
