-----------------------------------
-- Pure system tests for casket lock finish pure plans (slice 6188).
-----------------------------------

describe('casket lock finish pure plan', function()
    local STYLE_BLUE  = 965
    local STYLE_BROWN = 966

    -- chest style: brown if roll <= 15 + kupowersBonus
    local function planChestStyle(roll, kupowersBonus)
        if roll <= 15 + kupowersBonus then
            return STYLE_BROWN
        end
        return STYLE_BLUE
    end

    local GUESS_INVALID = 0
    local GUESS_CORRECT = 1
    local GUESS_WRONG   = 2

    local function planGuess(inputNumber, correctNumber)
        if not (inputNumber > 9 and inputNumber < 100) then
            return GUESS_INVALID, 0
        end
        if inputNumber == correctNumber then
            return GUESS_CORRECT, 0
        end
        local isGreater = 0
        if inputNumber > correctNumber then
            isGreater = 1
        end
        return GUESS_WRONG, isGreater
    end

    local function planEvenOdd(digit)
        return digit % 2
    end

    -- three-number digit range message params
    local function planDigitTriple(digit)
        if digit <= 6 then
            return digit, digit + 1, digit + 2
        elseif digit == 9 then
            return digit - 2, digit - 1, digit
        end
        return digit - 1, digit, digit + 1
    end

    local function planItemPos(option)
        return bit.band(option, 0x7)
    end

    local function planSubOption(option)
        return bit.band(bit.rshift(option, 16), 0x3)
    end

    local function planItemsEmpty(i1, i2, i3, i4)
        return i1 == 0 and i2 == 0 and i3 == 0 and i4 == 0
    end

    -- first tools trade always hints; later trades need roll < 0.2
    local function planToolsCanHint(firstAttempt, tradeRoll)
        if firstAttempt == 0 or firstAttempt == nil then
            return true
        end
        return tradeRoll < 0.2
    end

    it('chest style brown vs blue', function()
        assert(planChestStyle(15, 0) == STYLE_BROWN)
        assert(planChestStyle(16, 0) == STYLE_BLUE)
        assert(planChestStyle(20, 5) == STYLE_BROWN) -- 15+5
        assert(planChestStyle(21, 5) == STYLE_BLUE)
    end)

    it('guess branches', function()
        local k, g = planGuess(5, 50)
        assert(k == GUESS_INVALID)
        k, g = planGuess(50, 50)
        assert(k == GUESS_CORRECT)
        k, g = planGuess(60, 50)
        assert(k == GUESS_WRONG and g == 1)
        k, g = planGuess(40, 50)
        assert(k == GUESS_WRONG and g == 0)
    end)

    it('even odd and digit triple', function()
        assert(planEvenOdd(4) == 0 and planEvenOdd(5) == 1)
        local a, b, c = planDigitTriple(3)
        assert(a == 3 and b == 4 and c == 5)
        a, b, c = planDigitTriple(9)
        assert(a == 7 and b == 8 and c == 9)
        a, b, c = planDigitTriple(8)
        assert(a == 7 and b == 8 and c == 9)
    end)

    it('item pos and empty', function()
        assert(planItemPos(3) == 3)
        assert(planSubOption(bit.lshift(2, 16) + 1) == 2)
        assert(planItemsEmpty(0, 0, 0, 0))
        assert(not planItemsEmpty(1, 0, 0, 0))
    end)

    it('tools can hint', function()
        assert(planToolsCanHint(0, 0.9))
        assert(planToolsCanHint(nil, 0.9))
        assert(planToolsCanHint(1, 0.1))
        assert(not planToolsCanHint(1, 0.2))
        assert(not planToolsCanHint(1, 0.5))
    end)
end)
