require('scripts/actions/mobskills/aura_of_persistence')

describe('Aura of Persistence mob skill', function()
    it('rejects an existing Defense Boost and applies a TP-scaled buff with its message', function()
        local args, message = nil, nil
        local hasDefenseBoost = false
        local tp = 1500
        local mob = {
            hasStatusEffect = function() return hasDefenseBoost end,
        }
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        local buffMove = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(...)
            args = { ... }
            return 321
        end

        local aura = require('scripts/actions/mobskills/aura_of_persistence')
        assert(aura.onMobSkillCheck({}, mob, {}) == 0)
        hasDefenseBoost = true
        assert(aura.onMobSkillCheck({}, mob, {}) == 1)
        hasDefenseBoost = false
        assert(aura.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        assert(args[2] == xi.effect.DEFENSE_BOOST and args[3] == 20 and args[4] == 0 and args[5] == 45 and message == 321)

        tp = 3000
        assert(aura.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(args[5] == 90)
    end)
end)
