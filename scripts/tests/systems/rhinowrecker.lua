require('scripts/actions/mobskills/rhinowrecker')
describe('Rhinowrecker mob skill', function()
    it('uses blunt physical plan and applies Defense Down after processing', function()
        local skillObj = require('scripts/actions/mobskills/rhinowrecker')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skillObj.onMobSkillCheck(target, mob, {}) == 0 and skillObj.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 3 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillObj.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 25 and statusParams[6] == 180)
    end)
end)
