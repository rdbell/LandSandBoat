require('scripts/actions/mobskills/sweet_breath')
describe('Sweet Breath mob skill', function()
    it('uses dark breath plan and sleep after processing', function()
        local skill = require('scripts/actions/mobskills/sweet_breath')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = {}
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.0909 and params.damageCap == 500 and params.element == xi.element.DARK and statusParams == nil)
        math.random = function(a, b) assert(a == 15 and b == 60); return 30 end
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 200 and statusParams[3] == xi.effect.SLEEP_I and statusParams[6] == 30)
    end)
end)
