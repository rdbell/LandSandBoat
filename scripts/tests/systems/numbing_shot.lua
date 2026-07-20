require('scripts/actions/mobskills/numbing_shot')

describe('Numbing Shot mob skill', function()
    it('uses its ranged plan and paralyzes only after processing', function()
        local shot = require('scripts/actions/mobskills/numbing_shot')
        local move, process, status, duration = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration
        local params, damage, paralysis = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) paralysis = { ... } end
        xi.mobskills.calculateDuration = function(tp, min, max)
            assert(tp == 1500 and min == 60 and max == 180)
            return 120
        end
        assert(shot.onMobSkillCheck(target, mob, skill) == 0 and shot.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and paralysis == nil)
        xi.mobskills.processDamage = function() return true end
        shot.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = move, process, status, duration
        assert(damage[1] == 123 and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
        assert(paralysis[3] == xi.effect.PARALYSIS and paralysis[4] == 30 and paralysis[5] == 0 and paralysis[6] == 120)
    end)
end)
