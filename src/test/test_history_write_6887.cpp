#include "test_history_write_6887.h"

#include "map/char_history_write.h"

#include <iostream>

auto runHistoryWrite6887SelfTests() -> bool
{
    const historyloadhelpers::CharHistory history{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
    const bool ok = historywritehelpers::MakeHistoryWritePlan(false, history) == historywritehelpers::HistoryWritePlan{} &&
                    historywritehelpers::MakeHistoryWritePlan(true, history) == historywritehelpers::HistoryWritePlan{
                                                                          .persist = true,
                                                                          .history = history,
                                                                      };
    if (!ok)
    {
        std::cerr << "history write 6887 self-test failed\n";
    }
    return ok;
}
