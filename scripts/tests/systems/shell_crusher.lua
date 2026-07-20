require('scripts/actions/mobskills/shell_crusher')
describe('Shell Crusher mob skill', function()
    it('uses blunt physical plan and applies TP-scaled Defense Down', function()
        local crusher = require('scripts/actions/mobskills/shell_crusher')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(crusher.onMobSkillCheck(target, mob, skill) == 0 and crusher.onMobWeaponSkill(mob, target, skill, {}) == 50)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        crusher.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 50 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 25 and statusParams[6] == 180)
    end)
end)
