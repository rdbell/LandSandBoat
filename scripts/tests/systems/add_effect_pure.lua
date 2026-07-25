-----------------------------------
-- Pure system tests for add-effect damage/status dual-wire helpers (slice 6702).
-- Calls production xi.combat.action pure exports.
-- Goldens match internal/addeffectdamage and internal/addeffectstatus.
-----------------------------------

require('scripts/globals/combat/action_additional_effect_damage')
require('scripts/globals/combat/action_additional_effect_status')

local act = xi.combat.action
local el = xi.element
local se = xi.subEffect
local eff = xi.effect

describe('add-effect damage defaults and validate', function()
    it('pins chance, lowest resist, and element animations', function()
        assert(act.addEffectDamageDefaultChance == 100)
        assert(math.abs(act.addEffectDamageDefaultLowestResist - 0.125) < 1e-12)
        assert(act.animationForMagicalElement(el.NONE) == se.LIGHT_DAMAGE)
        assert(act.animationForMagicalElement(el.FIRE) == se.FIRE_DAMAGE)
        assert(act.animationForMagicalElement(el.DARK) == se.DARKNESS_DAMAGE)
        assert(xi.msg.basic.ADD_EFFECT_DMG == 163)
        assert(xi.msg.basic.ADD_EFFECT_HEAL == 384)
    end)

    it('applies empty fed defaults and element animation', function()
        local p = act.validateAddEffectDamageParams({})
        assert(p.chance == 100)
        assert(p.attackType == xi.attackType.SPECIAL)
        assert(p.magicalElement == el.NONE)
        assert(p.lowestResist == 0.125)
        assert(p.animation == se.LIGHT_DAMAGE)
        assert(p.messageDamage == 163)
        assert(p.messageHeal == 384)
        assert(p.targetStat == 0)

        p = act.validateAddEffectDamageParams({ magicalElement = el.FIRE, actorStat = 5 })
        assert(p.animation == se.FIRE_DAMAGE)
        assert(p.targetStat == 5) -- defaults to actorStat

        p = act.validateAddEffectDamageParams({ chance = 50, animation = 99, lowestResist = 0.5 })
        assert(p.chance == 50 and p.animation == 99 and p.lowestResist == 0.5)
    end)
end)

describe('enspell and product pure', function()
    it('detects enspell effects from the shared list', function()
        assert(#act.enspellEffects == 14)
        assert(act.enspellEffects[1] == eff.ENFIRE)
        assert(act.enspellEffects[14] == eff.ENDARK)
        assert(not act.hasEnspellFromParams(function() return false end))
        assert(act.hasEnspellFromParams(function(id) return id == eff.ENFIRE end))
        assert(act.hasEnspellFromParams({ [eff.ENDARK] = true }))
        assert(not act.hasEnspellFromParams({ [eff.POISON] = true }))
    end)

    it('gates product and floors multipliers', function()
        assert(act.procMiss(50, 50) == false)
        assert(act.procMiss(51, 50) == true)
        assert(act.resistBelowFloor(0.1, 0.125) == true)
        assert(act.resistBelowFloor(0.125, 0.125) == false)

        assert(act.baseDamageFromParams({ basePower = 10, actorStatMod = 50, targetStatMod = 20 }) == 40)

        assert(act.addEffectDamageProductFromParams({ hasEnspell = true, base = 100 }) == 0)
        assert(act.addEffectDamageProductFromParams({ procMiss = true, base = 100 }) == 0)
        assert(act.addEffectDamageProductFromParams({ limitUndead = true, base = 100 }) == 0)
        assert(act.addEffectDamageProductFromParams({ resistBelowFloor = true, base = 100 }) == 0)

        -- Identity product
        local dmg = act.addEffectDamageProductFromParams({ base = 100 })
        assert(dmg == 100)

        -- Floor chain: 100 * 0.5 = 50; * 1.1 = 55
        dmg = act.addEffectDamageProductFromParams({
            base = 100, absorption = 0.5, nullification = 1.1,
        })
        assert(dmg == 55)

        -- Absorb negative: 100 * -1 = -100
        dmg = act.addEffectDamageProductFromParams({ base = 100, absorption = -1 })
        assert(dmg == -100)

        assert(act.drainClamp(50, 30, false) == 30)
        assert(act.drainClamp(50, 30, true) == 50)
        assert(act.drainClamp(-5, 30, false) == 0)
    end)
end)

describe('add-effect status defaults and gates', function()
    it('pins status defaults and validate', function()
        assert(act.addEffectStatusDefaultChance == 100)
        assert(act.addEffectStatusDefaultDuration == 120)
        local anim, msg = act.defaultsForStatusEffect(eff.POISON)
        assert(anim == se.POISON and msg == xi.msg.basic.ADD_EFFECT_STATUS)
        anim, msg = act.defaultsForStatusEffect(eff.NONE)
        assert(anim == se.DARKNESS_DAMAGE and msg == xi.msg.basic.ADD_EFFECT_DISPEL)
        anim, msg = act.defaultsForStatusEffect(9999)
        assert(anim == 0 and msg == 0)

        local p = act.validateAddEffectStatusParams({})
        assert(p.chance == 100)
        assert(p.effectId == eff.NONE)
        assert(p.duration == 120)
        assert(p.animation == se.DARKNESS_DAMAGE)
        assert(p.message == xi.msg.basic.ADD_EFFECT_DISPEL)

        p = act.validateAddEffectStatusParams({ effectId = eff.POISON, power = 5, duration = 60 })
        assert(p.effectId == eff.POISON and p.power == 5 and p.duration == 60)
        assert(p.animation == se.POISON)
    end)

    it('composes enhancement/enfeeblement/dispel gates', function()
        assert(act.enhancementAppliesFromParams({
            effectId = eff.POISON, addStatusOK = true,
        }))
        assert(not act.enhancementAppliesFromParams({
            hasEnspell = true, effectId = eff.POISON, addStatusOK = true,
        }))
        assert(not act.enhancementAppliesFromParams({
            effectId = eff.NONE, addStatusOK = true,
        }))
        assert(not act.enhancementAppliesFromParams({
            effectId = eff.POISON, procMiss = true, addStatusOK = true,
        }))
        assert(not act.enhancementAppliesFromParams({
            effectId = eff.POISON, nullified = true, addStatusOK = true,
        }))
        assert(not act.enhancementAppliesFromParams({
            effectId = eff.POISON, addStatusOK = false,
        }))

        assert(act.enfeeblementAppliesFromParams({
            effectId = eff.POISON, addStatusOK = true,
        }))
        assert(not act.enfeeblementAppliesFromParams({
            effectId = eff.POISON, immune = true, addStatusOK = true,
        }))
        assert(not act.enfeeblementAppliesFromParams({
            effectId = eff.POISON, resistRateFail = true, addStatusOK = true,
        }))

        assert(act.dispelAppliesFromParams({
            effectId = eff.NONE, hasDispelable = true,
        }))
        assert(not act.dispelAppliesFromParams({
            effectId = eff.POISON, hasDispelable = true,
        }))
        assert(not act.dispelAppliesFromParams({
            effectId = eff.NONE, hasDispelable = false,
        }))

        assert(act.enfeebleDurationFromParams(120, 0.5) == 60)
        assert(act.enfeebleDurationFromParams(100, 0.333) == 33)
    end)
end)
