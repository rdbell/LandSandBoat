require('scripts/actions/mobskills/dulling_arrow')

describe('Dulling Arrow mob skill', function()
    it('uses its ranged critical plan and applies INT Down only after processing', function()
        local arrow = require('scripts/actions/mobskills/dulling_arrow')
        local rangedMove, processDamage, statusMove = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, intDown = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) intDown = { effect, power, tick, duration } end
        assert(arrow.onMobSkillCheck({}, {}, {}) == 0)
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and params.criticalChance[1] == 0.1 and params.criticalChance[2] == 0.3 and params.criticalChance[3] == 0.5)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and intDown == nil)
        xi.mobskills.processDamage = function() return true end
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = rangedMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
        assert(intDown[1] == xi.effect.INT_DOWN and intDown[2] == 10 and intDown[3] == 0 and intDown[4] == 120)
    end)
end)
