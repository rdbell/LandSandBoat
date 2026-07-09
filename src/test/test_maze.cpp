/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_maze.h"

#include "map/maze.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "maze self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runMazeSelfTests() -> bool
{
    bool   ok = true;
    maze_t maze{};

    ok = expect(!maze.hasVoucher(0), "voucher zero defaults false") && ok;
    ok = expect(!maze.hasVoucher(1), "voucher one defaults false") && ok;
    ok = expect(!maze.hasVoucher(64), "voucher max defaults false") && ok;
    ok = expect(!maze.hasVoucher(65), "voucher above max defaults false") && ok;
    ok = expect(!maze.hasRune(0), "rune zero defaults false") && ok;
    ok = expect(!maze.hasRune(1), "rune one defaults false") && ok;
    ok = expect(!maze.hasRune(512), "rune max defaults false") && ok;
    ok = expect(!maze.hasRune(513), "rune above max defaults false") && ok;

    maze.learnVoucher(0);
    maze.learnVoucher(65);
    maze.learnRune(0);
    maze.learnRune(513);
    ok = expect(!maze.hasVoucher(0) && !maze.hasVoucher(65), "invalid vouchers ignored") && ok;
    ok = expect(!maze.hasRune(0) && !maze.hasRune(513), "invalid runes ignored") && ok;

    maze.learnVoucher(1);
    maze.learnVoucher(64);
    maze.learnVoucher(1);
    ok = expect(maze.hasVoucher(1), "voucher one learned idempotently") && ok;
    ok = expect(maze.hasVoucher(64), "voucher max learned") && ok;
    ok = expect(!maze.hasVoucher(2), "other voucher remains false") && ok;
    ok = expect(!maze.hasRune(1) && !maze.hasRune(64), "voucher state independent of runes") && ok;

    maze.learnRune(1);
    maze.learnRune(512);
    maze.learnRune(1);
    ok = expect(maze.hasRune(1), "rune one learned idempotently") && ok;
    ok = expect(maze.hasRune(512), "rune max learned") && ok;
    ok = expect(!maze.hasRune(2), "other rune remains false") && ok;
    ok = expect(maze.hasVoucher(1) && maze.hasVoucher(64), "rune learning preserves vouchers") && ok;

    return ok;
}
