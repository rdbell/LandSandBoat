require('scripts/actions/mobskills/aerial_wheel')

describe('Aerial Wheel mob skill', function()
    it('uses its ranged plan, clamps distance scaling, and applies Stun only after processing', function()
        local params, damage, stun = nil, nil, nil
        local distance = 5.5
        local mob = {
            getWeaponDmg = function() return 77 end,
            checkDistance = function() return distance end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobRangedMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) stun = { ... } end

        local wheel = require('scripts/actions/mobskills/aerial_wheel')
        assert(wheel.onMobSkillCheck({}, {}, {}) == 0)
        assert(wheel.onMobWeaponSkill(mob, target, {}, {}) == 246)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil and stun == nil)

        distance = 15
        xi.mobskills.processDamage = function() return true end
        assert(wheel.onMobWeaponSkill(mob, target, {}, {}) == 369)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 369 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(stun[3] == xi.effect.STUN and stun[4] == 1 and stun[5] == 0 and stun[6] == 4)
    end)
end)
