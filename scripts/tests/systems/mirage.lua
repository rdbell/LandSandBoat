require('scripts/actions/mobskills/mirage')

describe('Mirage mob skill', function()
    it('rejects active Evasion Boost and applies its fixed self-buff', function()
        local mirage = require('scripts/actions/mobskills/mirage')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local active = false
        local mob = {
            hasStatusEffect = function(_, effect)
                return active and effect == xi.effect.EVASION_BOOST
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end

        assert(mirage.onMobSkillCheck({}, mob, skill) == 0)
        active = true
        assert(mirage.onMobSkillCheck({}, mob, skill) == 1)
        active = false
        assert(mirage.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == mob and params[2] == xi.effect.EVASION_BOOST and params[3] == 40 and params[4] == 0 and params[5] == 180)
        assert(message == 456)
    end)
end)
