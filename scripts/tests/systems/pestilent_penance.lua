require('scripts/actions/mobskills/pestilent_penance')

describe('Pestilent Penance mob skill', function()
    it('uses its dark magical plan and applies Plague after processing', function()
        local penance = require('scripts/actions/mobskills/pestilent_penance')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(penance.onMobSkillCheck(target, mob, {}) == 0 and penance.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.element == xi.element.DARK)
        assert(params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        penance.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 10 and statusParams[5] == 0 and statusParams[6] == 120)
    end)
end)
