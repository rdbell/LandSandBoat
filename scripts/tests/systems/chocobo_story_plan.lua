-----------------------------------
-- Pure system tests for chocobo story menu/learn plans (slice 6163).
-----------------------------------

describe('chocobo story pure plan', function()
    local KI_IMPATIENT = 837
    local KI_CURIOUS   = 838
    local KI_WORRISOME = 839
    local KI_YOUTHFUL  = 840
    local KI_HAPPY     = 841
    local KI_DILIGENT  = 850

    local function planStoryMask(hasKI)
        local storyMask = 0x7FFFFFFF + (-bit.lshift(1, 0)) -- random chitchat always
        local bits = {
            [KI_IMPATIENT] = 1,
            [KI_CURIOUS]   = 2,
            [KI_WORRISOME] = 3,
            [KI_YOUTHFUL]  = 4,
            [KI_HAPPY]     = 5,
            [KI_DILIGENT]  = 6,
        }
        for ki, bitn in pairs(bits) do
            if hasKI[ki] then
                storyMask = storyMask + (-bit.lshift(1, bitn))
            end
        end
        return storyMask
    end

    local function bitClear(mask, n)
        return bit.band(mask, bit.lshift(1, n)) == 0
    end

    local function storyIndex(option)
        return bit.rshift(option, 8)
    end

    -- Story ability enum used in handleStoryUpdate (0-based, not breeding Ability)
    local STORY_ABILITY = {
        [4] = 0, -- gallop
        [5] = 1, -- canter
        [6] = 2, -- burrow
        [7] = 3, -- bore
        [8] = 4, -- auto regen
        [9] = 5, -- treasure finder
    }

    local function planStoryLearn(option, roll)
        local idx = storyIndex(option)
        local ability = STORY_ABILITY[idx]
        local learned = roll >= 1 and roll <= 25
        return {
            storyIndex = idx,
            ability = ability,
            learned = learned,
        }
    end

    it('story mask always enables chitchat', function()
        local m = planStoryMask({})
        assert(bitClear(m, 0))
        assert(not bitClear(m, 1))
    end)

    it('story mask enables KI stories', function()
        local m = planStoryMask({ [KI_IMPATIENT] = true, [KI_DILIGENT] = true })
        assert(bitClear(m, 0) and bitClear(m, 1) and bitClear(m, 6))
        assert(not bitClear(m, 2))
    end)

    it('story learn chance and ability map', function()
        local r = planStoryLearn(bit.lshift(4, 8), 25)
        assert(r.storyIndex == 4 and r.ability == 0 and r.learned)
        r = planStoryLearn(bit.lshift(9, 8), 26)
        assert(r.storyIndex == 9 and r.ability == 5 and not r.learned)
        r = planStoryLearn(bit.lshift(8, 8), 1)
        assert(r.ability == 4 and r.learned)
    end)
end)
