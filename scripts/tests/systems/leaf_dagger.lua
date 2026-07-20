require('scripts/actions/mobskills/leaf_dagger')

describe('Leaf Dagger mob skill', function()
    it('uses its ranged piercing plan and applies level-scaled Poison only after processing', function()
        local leafDagger = require('scripts/actions/mobskills/leaf_dagger')
        local move, process, status = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end, getMainLvl = function() return 55 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effects[#effects + 1] = { ... } end

        assert(leafDagger.onMobSkillCheck(target, mob, {}) == 0)
        assert(leafDagger.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(leafDagger.onMobWeaponSkill(mob, target, {}, {}) == 123)
        mob.getMainLvl = function() return 1 end
        assert(leafDagger.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(#effects == 2)
        assert(effects[1][1] == mob and effects[1][2] == target and effects[1][3] == xi.effect.POISON)
        assert(effects[1][4] == 5.5 and effects[1][5] == 3 and effects[1][6] == 90)
        assert(effects[2][4] == 1 and effects[2][5] == 3 and effects[2][6] == 90)
    end)
end)
