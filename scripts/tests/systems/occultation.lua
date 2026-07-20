require('scripts/actions/mobskills/occultation')

describe('Occultation mob skill', function()
    it('applies Copy Image with NM-random shadow count and returns Blink', function()
        local occultation = require('scripts/actions/mobskills/occultation')
        local buff, random = xi.mobskills.mobBuffMove, math.random
        local params, message, notorious = nil, nil, false
        local mob = {
            isNM = function() return notorious end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        assert(occultation.onMobSkillCheck({}, mob, skill) == 0)
        assert(occultation.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BLINK)
        assert(params[2] == xi.effect.COPY_IMAGE and params[3] == 1 and params[4] == 0 and params[5] == 300 and params[6] == 0 and params[7] == 10)
        notorious = true
        math.random = function(low, high)
            assert(low == 10 and high == 25)
            return 18
        end
        assert(occultation.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BLINK)
        xi.mobskills.mobBuffMove, math.random = buff, random
        assert(params[7] == 18 and message == 456)
    end)
end)
