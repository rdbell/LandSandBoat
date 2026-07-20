describe('Frigid Shuffle mob skill', function()
    it('forwards Apkallu admission, misses unfaced targets, and applies random-duration Paralysis', function()
        local shuffle = require('scripts/actions/mobskills/frigid_shuffle')
        local canUse, status, random = xi.apkallu.canUseAbility, xi.mobskills.mobStatusEffectMove, math.random
        local message, effect = nil, nil
        local mob = {}
        local target = { isFacing = function() return false end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.apkallu.canUseAbility = function(_, threshold) effect = threshold; return 1 end
        assert(shuffle.onMobSkillCheck(target, mob, skill) == 1 and effect == 30 and shuffle.onMobWeaponSkill(mob, target, skill, {}) == nil and message == xi.msg.basic.SKILL_MISS)
        target.isFacing = function() return true end
        math.random = function(minimum, maximum) assert(minimum == 60 and maximum == 120); return 90 end
        xi.mobskills.mobStatusEffectMove = function(_, _, effectID, power, tick, duration) assert(effectID == xi.effect.PARALYSIS and power == 50 and tick == 0 and duration == 90); return 777 end
        assert(shuffle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PARALYSIS and message == 777)
        xi.apkallu.canUseAbility, xi.mobskills.mobStatusEffectMove, math.random = canUse, status, random
    end)
end)
