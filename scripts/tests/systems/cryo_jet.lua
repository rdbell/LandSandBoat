require('scripts/actions/mobskills/cryo_jet')

describe('Cryo Jet mob skill', function()
    it('uses level-dependent Ice breath parameters and resets nuclear waste', function()
        local cryoJet = require('scripts/actions/mobskills/cryo_jet')
        local originalBreathMove = xi.mobskills.mobBreathMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, nuclearWaste = nil, nil
        local mob = {
            getMainLvl = function() return 64 end,
            setLocalVar = function(_, name, value) nuclearWaste = { name, value } end,
        }
        local target = {}

        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(cryoJet.onMobSkillCheck(target, mob, nil) == 0)
        assert(cryoJet.onMobWeaponSkill(mob, target, nil, nil) == 123)

        xi.mobskills.mobBreathMove = originalBreathMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(params.percentMultipier == 0.05 and params.damageCap == 490 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.ICE)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.ICE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(nuclearWaste[1] == 'nuclearWaste' and nuclearWaste[2] == 0)
    end)

    it('applies Paralysis and removes Elemental Resistance Down only after damage processing', function()
        local cryoJet = require('scripts/actions/mobskills/cryo_jet')
        local originalBreathMove = xi.mobskills.mobBreathMove
        local originalProcessDamage = xi.mobskills.processDamage
        local originalStatusEffectMove = xi.mobskills.mobStatusEffectMove
        local paralysis, removed, damage = nil, nil, nil
        local mob = {
            getMainLvl = function() return 65 end,
            setLocalVar = function() end,
        }
        local target = {
            takeDamage = function(_, value) damage = value end,
            hasStatusEffect = function(_, effect) return effect == xi.effect.ELEMENTALRES_DOWN end,
            delStatusEffectSilent = function(_, effect) removed = effect end,
        }

        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            assert(value.damageCap == 750)
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(source, recipient, effect, power, tick, duration)
            paralysis = { source, recipient, effect, power, tick, duration }
        end

        assert(cryoJet.onMobWeaponSkill(mob, target, nil, nil) == 123)

        xi.mobskills.mobBreathMove = originalBreathMove
        xi.mobskills.processDamage = originalProcessDamage
        xi.mobskills.mobStatusEffectMove = originalStatusEffectMove

        assert(damage == 123)
        assert(paralysis[1] == mob and paralysis[2] == target and paralysis[3] == xi.effect.PARALYSIS)
        assert(paralysis[4] == 15 and paralysis[5] == 3 and paralysis[6] == 120)
        assert(removed == xi.effect.ELEMENTALRES_DOWN)
    end)
end)
