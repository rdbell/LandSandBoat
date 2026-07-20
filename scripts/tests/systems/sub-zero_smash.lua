require('scripts/actions/mobskills/sub-zero_smash')
describe('Sub-Zero Smash mob skill', function()
    it('uses blunt physical plan and applies Paralysis after processing', function()
        local smash = require('scripts/actions/mobskills/sub-zero_smash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(smash.onMobSkillCheck(target, mob, {}) == 0 and smash.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.fTP[1] == 1 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        smash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 50 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 10 and statusParams[6] == 100)
    end)
end)
