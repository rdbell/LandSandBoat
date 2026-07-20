require('scripts/actions/mobskills/thunder_breath')
describe('Thunder Breath mob skill', function()
    it('admits in Dynamis/battlefield and uses thunder breath plan', function()
        local skill = require('scripts/actions/mobskills/thunder_breath')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            isInDynamis = function() return false end,
            hasStatusEffect = function(_, e) return false end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        target.isInDynamis = function() return true end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.125 and params.damageCap == 700 and params.bonusDamage == 78 and params.element == xi.element.THUNDER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
