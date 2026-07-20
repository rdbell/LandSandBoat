require('scripts/actions/mobskills/snatch_morsel')
describe('Snatch Morsel mob skill', function()
    it('strips FOOD with erase or miss message', function()
        local snatch = require('scripts/actions/mobskills/snatch_morsel')
        local hasFood, stripped, message = false, false, nil
        local target = {
            hasStatusEffect = function(_, e) return e == xi.effect.FOOD and hasFood end,
            delStatusEffectSilent = function(_, e) if e == xi.effect.FOOD then stripped = true end end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(snatch.onMobSkillCheck(target, {}, skill) == 0)
        assert(snatch.onMobWeaponSkill({}, target, skill, {}) == xi.effect.FOOD)
        assert(message == xi.msg.basic.SKILL_MISS and not stripped)
        hasFood = true
        assert(snatch.onMobWeaponSkill({}, target, skill, {}) == xi.effect.FOOD)
        assert(stripped and message == xi.msg.basic.SKILL_ERASE)
    end)
end)
