require('scripts/actions/mobskills/pirate_pummel')

describe('Pirate Pummel mob skill', function()
    it('uses its twofold piercing plan and applies Burn after processing', function()
        local pummel = require('scripts/actions/mobskills/pirate_pummel')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(pummel.onMobSkillCheck(target, mob, {}) == 0 and pummel.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2 and params.fTP[1] == 0.3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        pummel.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(statusParams[3] == xi.effect.BURN and statusParams[4] == 1 and statusParams[5] == 0 and statusParams[6] == 20)
    end)
end)
