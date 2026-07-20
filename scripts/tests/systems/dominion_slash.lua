require('scripts/actions/mobskills/dominion_slash')

describe('Dominion Slash mob skill', function()
    it('uses its physical plan and gates Silence and dispel behind damage processing and resistance', function()
        local slash = require('scripts/actions/mobskills/dominion_slash')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local statusMove, resistRate = xi.mobskills.mobStatusEffectMove, xi.combat.magicHitRate.calculateResistRate
        local params, damage, silence, dispels, rate = nil, nil, nil, 0, 0
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function() dispels = dispels + 1 end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            silence = { effect, power, tick, duration }
        end
        xi.combat.magicHitRate.calculateResistRate = function() return rate end

        assert(slash.onMobSkillCheck({}, {}, {}) == 0)
        assert(slash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 3.25 and params.fTP[2] == 3.25 and params.fTP[3] == 3.25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and silence == nil and dispels == 0)

        xi.mobskills.processDamage = function() return true end
        rate = 0.24
        assert(slash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(silence[1] == xi.effect.SILENCE and silence[2] == 1 and silence[3] == 0 and silence[4] == 60)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(dispels == 0)

        rate = 0.25
        assert(slash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        xi.mobskills.mobStatusEffectMove, xi.combat.magicHitRate.calculateResistRate = statusMove, resistRate
        assert(dispels == 1)
    end)
end)
