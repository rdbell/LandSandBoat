require('scripts/actions/mobskills/shell_bash')
describe('Shell Bash mob skill', function()
    it('uses blunt physical plan and applies Stun after processing', function()
        local bash = require('scripts/actions/mobskills/shell_bash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(bash.onMobSkillCheck(target, mob, {}) == 0 and bash.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        bash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 40 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
