require('scripts/globals/gambits')

-- The gambit DSL vocabulary is transcribed independently into C++ (G_TARGET,
-- G_CONDITION, ...) and into OmegaXI's internal/gambits. These pins catch a
-- renumbering here that would silently desync those ports.

local function countEntries(enumTable)
    local count = 0
    for _ in pairs(enumTable) do
        count = count + 1
    end

    return count
end

local function assertContiguousFromZero(enumTable, expectedCount, label)
    assert(countEntries(enumTable) == expectedCount, label .. ' count')

    local seen = {}
    for name, value in pairs(enumTable) do
        assert(type(value) == 'number', label .. '.' .. name)
        assert(value >= 0 and value < expectedCount, label .. '.' .. name .. '=' .. tostring(value))
        assert(not seen[value], label .. ' duplicate value ' .. tostring(value))
        seen[value] = true
    end
end

describe('Gambit DSL targets', function()
    it('numbers all fourteen targets contiguously from zero', function()
        assertContiguousFromZero(ai.target, 14, 'ai.target')
    end)

    it('pins the target values', function()
        assert(ai.target.SELF == 0)
        assert(ai.target.TARGET == 2)
        assert(ai.target.TOP_ENMITY == 8)
        assert(ai.target.CURILLA == 9)
        assert(ai.target.TRIGGER_SELF_ACTION_TARGET == 12)
        assert(ai.target.TRIGGER_TARGET_ACTION_SELF == 13)
    end)

    it('aliases ai.t to ai.target', function()
        assert(ai.t == ai.target)
    end)
end)

describe('Gambit DSL conditions', function()
    it('numbers all forty conditions contiguously from zero', function()
        assertContiguousFromZero(ai.condition, 40, 'ai.condition')
    end)

    it('pins the condition values', function()
        assert(ai.condition.ALWAYS == 0)
        assert(ai.condition.HPP_LT == 1)
        assert(ai.condition.STATUS == 9)
        assert(ai.condition.RANDOM == 22)
        assert(ai.condition.TIMER == 39)
    end)

    it('aliases ai.c to ai.condition', function()
        assert(ai.c == ai.condition)
    end)
end)

describe('Gambit DSL reactions', function()
    it('numbers all seven reactions contiguously from zero', function()
        assertContiguousFromZero(ai.reaction, 7, 'ai.reaction')
    end)

    it('pins the reaction values', function()
        assert(ai.reaction.ATTACK == 0)
        assert(ai.reaction.MA == 2)
        assert(ai.reaction.WS == 4)
        assert(ai.reaction.ANIM_STRING == 6)
    end)

    it('aliases ai.r to ai.reaction', function()
        assert(ai.r == ai.reaction)
    end)
end)

describe('Gambit DSL selects', function()
    it('numbers all twenty selects contiguously from zero', function()
        assertContiguousFromZero(ai.select, 20, 'ai.select')
    end)

    it('pins the select values', function()
        assert(ai.select.HIGHEST == 0)
        assert(ai.select.SPECIFIC == 2)
        assert(ai.select.MB_ELEMENT == 4)
        assert(ai.select.RANDOM_ANIMATION == 19)
    end)

    it('aliases ai.s to ai.select', function()
        assert(ai.s == ai.select)
    end)
end)

describe('Gambit DSL TP triggers', function()
    it('numbers all five TP triggers contiguously from zero', function()
        assertContiguousFromZero(ai.tp, 5, 'ai.tp')
    end)

    it('pins the TP trigger values', function()
        assert(ai.tp.ASAP == 0)
        assert(ai.tp.RANDOM == 1)
        assert(ai.tp.OPENER == 2)
        assert(ai.tp.CLOSER == 3)
        assert(ai.tp.CLOSER_UNTIL_TP == 4)
    end)
end)

describe('Gambit DSL logic', function()
    it('pins AND to zero and OR to one', function()
        assert(ai.logic.AND == 0)
        assert(ai.logic.OR.value == 1)
    end)

    it('aliases ai.l to ai.logic', function()
        assert(ai.l == ai.logic)
    end)

    it('builds a condition group when called', function()
        local first  = { condition = ai.condition.ALWAYS }
        local second = { condition = ai.condition.HPP_LT, value = 50 }
        local group  = ai.logic.OR(first, second)

        assert(group.logic == 1)
        assert(#group.conditions == 2)
        assert(group.conditions[1] == first)
        assert(group.conditions[2] == second)
    end)

    it('builds an empty group with no conditions', function()
        local group = ai.logic.OR()

        assert(group.logic == 1)
        assert(#group.conditions == 0)
    end)

    it('rejects a non-table condition', function()
        assert(not pcall(function()
            ai.logic.OR({ condition = ai.condition.ALWAYS }, 5)
        end))
    end)
end)
