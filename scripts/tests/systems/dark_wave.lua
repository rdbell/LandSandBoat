require('scripts/actions/mobskills/dark_wave')

describe('Dark Wave mob skill', function()
    it('is always available', function()
        local wave = require('scripts/actions/mobskills/dark_wave')

        assert(wave.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses stronger Bio at night and its fixed effect parameters', function()
        local hour, requests = nil, {}
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        stub('VanadielHour', function() return hour end)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration, subPower, attackReduction)
            table.insert(requests, { effect, power, tick, duration, subPower, attackReduction })
        end

        local wave = require('scripts/actions/mobskills/dark_wave')
        hour = 5
        assert(wave.onMobWeaponSkill({}, {}, {}, {}) == xi.effect.BIO)
        hour = 6
        assert(wave.onMobWeaponSkill({}, {}, {}, {}) == xi.effect.BIO)
        hour = 17
        assert(wave.onMobWeaponSkill({}, {}, {}, {}) == xi.effect.BIO)
        hour = 18
        assert(wave.onMobWeaponSkill({}, {}, {}, {}) == xi.effect.BIO)

        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(requests[1][1] == xi.effect.BIO and requests[1][2] == 20)
        assert(requests[2][2] == 8 and requests[3][2] == 8 and requests[4][2] == 20)
        for _, request in ipairs(requests) do
            assert(request[3] == 3 and request[4] == 60)
            assert(request[5] == 0 and request[6] == 25)
        end
    end)
end)
