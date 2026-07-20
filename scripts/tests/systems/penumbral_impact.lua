require('scripts/actions/mobskills/penumbral_impact')

describe('Penumbral Impact mob skill', function()
    it('uses its dark magical plan and damages only after processing', function()
        local impact = require('scripts/actions/mobskills/penumbral_impact')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(impact.onMobSkillCheck(target, mob, {}) == 0 and impact.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.element == xi.element.DARK)
        assert(params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        impact.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
