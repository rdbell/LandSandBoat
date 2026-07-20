require('scripts/actions/mobskills/magma_hoplon')

describe('Magma Hoplon mob skill', function()
    it('sets its Stoneskin message, applies random Blaze Spikes, and makes present Stoneskin undispellable', function()
        local magmaHoplon = require('scripts/actions/mobskills/magma_hoplon')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local calls, randomCalls, message, removed, effect = {}, {}, nil, nil, nil
        local mob = { getStatusEffect = function() return effect end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            table.insert(calls, { ... })
            return 456
        end
        math.random = function(min, max)
            table.insert(randomCalls, { min, max })
            return 27
        end

        assert(magmaHoplon.onMobSkillCheck(nil, mob, skill) == 0)
        assert(magmaHoplon.onMobWeaponSkill(mob, nil, skill, {}) == xi.effect.STONESKIN)
        assert(message == 456 and #calls == 2 and #randomCalls == 1)
        assert(calls[1][1] == mob and calls[1][2] == xi.effect.STONESKIN and calls[1][3] == 1000 and calls[1][4] == 0 and calls[1][5] == 300)
        assert(calls[2][1] == mob and calls[2][2] == xi.effect.BLAZE_SPIKES and calls[2][3] == 27 and calls[2][4] == 0 and calls[2][5] == 180)
        assert(randomCalls[1][1] == 20 and randomCalls[1][2] == 30 and removed == nil)

        effect = { delEffectFlag = function(_, flag) removed = flag end }
        assert(magmaHoplon.onMobWeaponSkill(mob, nil, skill, {}) == xi.effect.STONESKIN)
        assert(#calls == 4 and #randomCalls == 2 and removed == xi.effectFlag.DISPELABLE)

        xi.mobskills.mobBuffMove = buff
        math.random = random
    end)
end)
