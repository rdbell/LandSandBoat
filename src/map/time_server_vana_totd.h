#pragma once

// Previous-TOTD state used by time_server's Vana'diel hourly phase. The
// snapshot changes only after a detected transition has been handled.
namespace timeservervanatotdhelpers
{

template <typename Totd>
struct Tracker
{
    Totd previous;
};

template <typename Totd>
constexpr auto HasChanged(const Tracker<Totd>& tracker, const Totd current) -> bool
{
    return tracker.previous != current;
}

template <typename Totd>
constexpr void MarkHandled(Tracker<Totd>& tracker, const Totd current)
{
    tracker.previous = current;
}

} // namespace timeservervanatotdhelpers
