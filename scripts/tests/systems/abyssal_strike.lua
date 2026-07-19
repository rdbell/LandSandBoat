require('scripts/actions/mobskills/abyssal_strike')

describe('Abyssal Strike mob skill', function()
    it('uses one-hit physical parameters and damages and stuns only after processing', function()
        local move, process, effectMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) stun = { ... } end
        local strike = require('scripts/actions/mobskills/abyssal_strike')
        assert(strike.onMobSkillCheck(target, mob, {}) == 0 and strike.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 4.7 and params.fTP[2] == 4.7 and params.fTP[3] == 4.7)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and stun == nil)
        xi.mobskills.processDamage = function() return true end
        assert(strike.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(stun[3] == xi.effect.STUN and stun[4] == 1 and stun[5] == 0 and stun[6] == 15)
    end)
end)
