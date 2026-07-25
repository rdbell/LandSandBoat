#include "test_history_load_6865.h"
#include "map/char_history_load.h"
#include <iostream>
auto runHistoryLoad6865SelfTests() -> bool
{
    const historyloadhelpers::CharHistory history{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    bool ok = historyloadhelpers::MakeHistoryLoadPlan(false, true, history) == historyloadhelpers::HistoryLoadPlan{};
    ok = historyloadhelpers::MakeHistoryLoadPlan(true, false, history) == historyloadhelpers::HistoryLoadPlan{} && ok;
    ok = historyloadhelpers::MakeHistoryLoadPlan(true, true, history) == historyloadhelpers::HistoryLoadPlan{ .apply = true, .history = history } && ok;
    if (!ok) std::cerr << "history load 6865 self-test failed\n";
    return ok;
}
