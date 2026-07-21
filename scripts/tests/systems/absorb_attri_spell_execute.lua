-----------------------------------
-- Pure system tests for doAbsorbAttriSpell full plan (slice 6118).
-- Source: scripts/globals/spells/absorb_spell.lua ~272–294
-----------------------------------

describe('doAbsorbAttriSpell pure plan', function()
    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_STEAL = 430 -- xi.msg.basic.MAGIC_STEAL (verify in enum if needed)

    local function use(p)
        local count = 0
        local firstOK = p.firstStealOK == true
        if not firstOK then
            return {
                returnCount = 0,
                setMsg = true,
                msg = MSG_MAGIC_NO_EFFECT,
                stealFirst = true,
                stealSecond = false,
            }
        end
        count = 1
        local stealSecond = false
        if p.hasNetherVoid then
            stealSecond = true
            if p.secondStealOK then
                count = count + 1
            end
        end
        return {
            returnCount = count,
            setMsg = true,
            msg = MSG_MAGIC_STEAL,
            stealFirst = true,
            stealSecond = stealSecond,
        }
    end

    it('no steal sets MAGIC_NO_EFFECT and count 0', function()
        local r = use({ firstStealOK = false })
        assert(r.returnCount == 0 and r.msg == MSG_MAGIC_NO_EFFECT)
        assert(r.stealFirst == true and r.stealSecond ~= true)
    end)

    it('single steal without nether void', function()
        local r = use({ firstStealOK = true, hasNetherVoid = false })
        assert(r.returnCount == 1 and r.msg == MSG_MAGIC_STEAL)
        assert(r.stealSecond ~= true)
    end)

    it('nether void second steal success counts 2', function()
        local r = use({ firstStealOK = true, hasNetherVoid = true, secondStealOK = true })
        assert(r.returnCount == 2 and r.msg == MSG_MAGIC_STEAL)
        assert(r.stealSecond == true)
    end)

    it('nether void second steal fail stays count 1', function()
        local r = use({ firstStealOK = true, hasNetherVoid = true, secondStealOK = false })
        assert(r.returnCount == 1 and r.msg == MSG_MAGIC_STEAL)
        assert(r.stealSecond == true) -- still attempted
    end)
end)
