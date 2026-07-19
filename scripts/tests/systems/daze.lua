require('scripts/actions/mobskills/daze')

describe('Daze mob skill', function()
    it('is always available', function()
        local daze = require('scripts/actions/mobskills/daze')

        assert(daze.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses its ranged plan and applies Stun only after processing', function()
        local params, damage, stun = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobRangedMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) stun = { ... } end

        local daze = require('scripts/actions/mobskills/daze')
        assert(daze.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 5 and params.fTP[2] == 5.5 and params.fTP[3] == 6)
        assert(params.accuracyModifier[1] == 150 and params.accuracyModifier[2] == 150 and params.accuracyModifier[3] == 150)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil and stun == nil)

        xi.mobskills.processDamage = function() return true end
        assert(daze.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
        assert(stun[3] == xi.effect.STUN and stun[4] == 1 and stun[5] == 0 and stun[6] == 4)
    end)
end)
