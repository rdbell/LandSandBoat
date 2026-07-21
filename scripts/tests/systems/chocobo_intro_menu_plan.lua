-----------------------------------
-- Pure system tests for chocobo INTRO_MENU_PT_3 flags (slice 6161).
-----------------------------------

describe('chocobo intro menu flags pure plan', function()
    local EGG, CHICK, ADULT_1 = 1, 2, 4

    -- LSB: menuFlags starts 0xFFFFFFFF then subtracts (adds negative) bit masks
    -- to clear bits for enabled options.
    local function planMenuFlags(p)
        local menuFlags = 0xFFFFFFFF

        -- Always enable bits 0,1,2 (clear them via + (-1<<n))
        menuFlags = menuFlags + (-bit.lshift(1, 0)) -- ask about condition
        menuFlags = menuFlags + (-bit.lshift(1, 1)) -- care for chocobo
        menuFlags = menuFlags + (-bit.lshift(1, 2)) -- set care schedule

        if p.stage > EGG and p.firstName == 'Chocobo' and p.lastName == 'Chocobo' then
            menuFlags = menuFlags + (-bit.lshift(1, 3)) -- name your chocobo
        end

        if p.whistleProg >= 4 then
            menuFlags = menuFlags + (-bit.lshift(1, 4))
            menuFlags = menuFlags + (-bit.lshift(1, 5))
            menuFlags = menuFlags + (-bit.lshift(1, 6))
            menuFlags = menuFlags + (-bit.lshift(1, 7))
        end

        if p.gmLevel >= 3 then
            menuFlags = menuFlags + (-bit.lshift(1, 26))
            menuFlags = menuFlags + (-bit.lshift(1, 27))
            menuFlags = menuFlags + (-bit.lshift(1, 28))
        end

        if p.stage >= ADULT_1 then
            menuFlags = menuFlags + (-bit.lshift(1, 29)) -- retire
        else
            menuFlags = menuFlags + (-bit.lshift(1, 30)) -- give up
        end

        menuFlags = menuFlags + (-bit.lshift(1, 31)) -- exit
        return menuFlags
    end

    local function bitClear(flags, n)
        return bit.band(flags, bit.lshift(1, n)) == 0
    end

    it('always enables condition care schedule bits', function()
        local f = planMenuFlags({
            stage = CHICK, firstName = 'Ace', lastName = 'G',
            whistleProg = 0, gmLevel = 0,
        })
        assert(bitClear(f, 0) and bitClear(f, 1) and bitClear(f, 2))
        assert(bitClear(f, 31)) -- exit
        assert(bitClear(f, 30)) -- give up (not adult)
        assert(not bitClear(f, 29)) -- retire off
    end)

    it('enables naming when default Chocobo past egg', function()
        local f = planMenuFlags({
            stage = CHICK, firstName = 'Chocobo', lastName = 'Chocobo',
            whistleProg = 0, gmLevel = 0,
        })
        assert(bitClear(f, 3))
        f = planMenuFlags({
            stage = EGG, firstName = 'Chocobo', lastName = 'Chocobo',
            whistleProg = 0, gmLevel = 0,
        })
        assert(not bitClear(f, 3))
    end)

    it('whistle and gm and retire bits', function()
        local f = planMenuFlags({
            stage = ADULT_1, firstName = 'Ace', lastName = 'G',
            whistleProg = 4, gmLevel = 3,
        })
        assert(bitClear(f, 4) and bitClear(f, 5) and bitClear(f, 6) and bitClear(f, 7))
        assert(bitClear(f, 26) and bitClear(f, 27) and bitClear(f, 28))
        assert(bitClear(f, 29) and not bitClear(f, 30))
    end)
end)
