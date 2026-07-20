require('scripts/actions/mobskills/sprout_smack')
describe('Sprout Smack mob skill', function()
    it('uses blunt physical plan and applies Slow after processing', function()
        local smack = require('scripts/actions/mobskills/sprout_smack')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(smack.onMobSkillCheck(target, mob, {}) == 0 and smack.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        smack.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 30 and statusParams[3] == xi.effect.SLOW and statusParams[4] == 3000 and statusParams[6] == 90)
    end)
end)
