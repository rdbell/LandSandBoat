require('scripts/actions/mobskills/megalith_throw')

describe('Megalith Throw mob skill', function()
    it('uses its ranged Slashing plan and applies Slow only after processing', function()
        local throw = require('scripts/actions/mobskills/megalith_throw')
        local rangedMove, processDamage, statusMove = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, slow = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) slow = { effect, power, tick, duration } end

        assert(throw.onMobSkillCheck({}, {}, {}) == 0)
        assert(throw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.5 and params.fTP[2] == 2.5 and params.fTP[3] == 2.5)
        assert(params.skipParry and params.skipGuard and params.skipBlock and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and slow == nil)
        xi.mobskills.processDamage = function() return true end
        assert(throw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = rangedMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(slow[1] == xi.effect.SLOW and slow[2] == 30 and slow[3] == 0 and slow[4] == 120)
    end)
end)
