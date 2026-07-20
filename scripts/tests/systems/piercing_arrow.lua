require('scripts/actions/mobskills/piercing_arrow')

describe('Piercing Arrow mob skill', function()
    it('uses its ranged piercing plan with ignore-defense and damages only after processing', function()
        local arrow = require('scripts/actions/mobskills/piercing_arrow')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(arrow.onMobSkillCheck(target, mob, {}) == 0 and arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.accuracyModifier[1] == 30 and params.ignoreDefense[1] == 0 and params.ignoreDefense[2] == 0.35 and params.ignoreDefense[3] == 0.5)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        arrow.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
