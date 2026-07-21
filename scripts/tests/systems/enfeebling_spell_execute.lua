-----------------------------------
-- Pure system tests for useEnfeeblingSpell full plan (slice 6112).
-- Source: scripts/globals/spells/enfeebling_spell.lua ~389–520
-----------------------------------

describe('useEnfeeblingSpell pure plan', function()
    local EFFECT_NONE = 255
    local EFFECT_POISON = 3
    local EFFECT_SLEEP_I = 2
    local EFFECT_BIND = 11
    local EFFECT_ADDLE = 21
    local EFFECT_PETRIFICATION = 7
    local EFFECT_SLOW = 13

    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_RESIST = 85
    local MSG_MAGIC_ENFEEB_IS = 236
    local MSG_MAGIC_ENFEEB = 237
    local MSG_MAGIC_BURST_ENFEEB = 268
    local MSG_MAGIC_BURST_ENFEEB_IS = 271
    local MSG_MAGIC_ERASE = 341
    local MSG_MAGIC_COMPLETE_RESIST = 655

    local SKILL_ENFEEBLING = 35

    local function effectiveResistRate(effect, rate, skillType, hasStymie, hasFealty)
        if effect == EFFECT_NONE or effect == 0 then
            return rate
        end
        if skillType == SKILL_ENFEEBLING and hasStymie then
            return 1
        end
        if hasFealty then
            return 0
        end
        return rate
    end

    local function bindDuration(d)
        if d < 5 then return 5 end
        if d > 60 then return 60 end
        return d
    end

    local function addleSub(cStat, tStat)
        local diff = math.floor((cStat - tStat) / 5)
        if diff < 0 then diff = 0 end
        if diff > 20 then diff = 20 end
        return 20 + diff
    end

    local function successMsg(offset, scCount)
        if scCount and scCount > 0 then
            return MSG_MAGIC_BURST_ENFEEB_IS - offset * 3
        end
        return MSG_MAGIC_ENFEEB_IS + offset
    end

    local function use(p)
        local effect = p.spellEffect or 0
        local dispel = (effect == EFFECT_NONE or effect == 0)

        if p.isImmune then
            return { returnEffect = effect, msg = MSG_MAGIC_COMPLETE_RESIST }
        end
        if p.isTraitResisted then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST, setActionModifier = true }
        end
        if p.isNullified then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT }
        end

        local rate = effectiveResistRate(effect, p.resistRate or 1, p.skillType or 0, p.hasStymie, p.hasFealty)
        if p.resistFailed then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST, runImmunobreak = true }
        end

        local potency = p.potency or 0
        local duration = math.floor((p.durationPreResist or 0) * rate)
        local tick = p.baseTick or 0
        local subPower = 0

        if effect == EFFECT_BIND then
            potency = p.targetSpeed or 0
            duration = bindDuration(duration)
        elseif effect == EFFECT_SLEEP_I then
            subPower = p.spellElement or 0
        elseif effect == EFFECT_ADDLE then
            subPower = addleSub(p.casterStat or 0, p.targetStat or 0)
        elseif effect == EFFECT_PETRIFICATION then
            if p.casterIsPC then subPower = 1 end
        elseif dispel then
            if not p.dispelAttempted or not p.dispelledEffect or p.dispelledEffect == 0 or p.dispelledEffect == EFFECT_NONE then
                return { returnEffect = EFFECT_NONE, msg = MSG_MAGIC_NO_EFFECT, dispelPath = true }
            end
            return { returnEffect = p.dispelledEffect, msg = MSG_MAGIC_ERASE, dispelPath = true }
        end

        if not p.addStatusOK then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, power = potency, duration = duration }
        end

        local r = {
            returnEffect = effect,
            msg = successMsg(p.messageOffset or 0, p.skillchainCount or 0),
            applyEffect = true,
            power = potency,
            duration = duration,
            tick = tick,
            subPower = subPower,
        }
        if p.skillType == SKILL_ENFEEBLING and p.hasStymie then
            r.deleteStymie = true
        end
        if p.skillchainCount and p.skillchainCount > 0 then
            r.triggerRoeBurst = true
        end
        return r
    end

    it('immune complete resist', function()
        local r = use({ spellEffect = EFFECT_POISON, isImmune = true })
        assert(r.msg == MSG_MAGIC_COMPLETE_RESIST)
    end)

    it('trait resist sets modifier path', function()
        local r = use({ spellEffect = EFFECT_POISON, isTraitResisted = true })
        assert(r.msg == MSG_MAGIC_RESIST and r.setActionModifier == true)
    end)

    it('nullified no effect', function()
        local r = use({ spellEffect = EFFECT_POISON, isNullified = true })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('resist fail runs immunobreak', function()
        local r = use({ spellEffect = EFFECT_SLOW, resistFailed = true })
        assert(r.runImmunobreak == true and r.msg == MSG_MAGIC_RESIST)
    end)

    it('success applies power duration tick', function()
        local r = use({
            spellEffect = EFFECT_POISON, skillType = SKILL_ENFEEBLING,
            potency = 10, durationPreResist = 90, resistRate = 1, baseTick = 3, addStatusOK = true,
        })
        assert(r.applyEffect == true and r.power == 10 and r.duration == 90 and r.tick == 3)
        assert(r.msg == MSG_MAGIC_ENFEEB_IS)
    end)

    it('duration scales by resist rate', function()
        local r = use({
            spellEffect = EFFECT_SLOW, durationPreResist = 180, resistRate = 0.5, addStatusOK = true,
        })
        assert(r.duration == 90)
    end)

    it('stymie forces rate 1 and delete flag', function()
        local r = use({
            spellEffect = EFFECT_POISON, skillType = SKILL_ENFEEBLING, hasStymie = true,
            durationPreResist = 100, resistRate = 0.25, addStatusOK = true,
        })
        assert(r.duration == 100 and r.deleteStymie == true)
    end)

    it('bind speed potency and duration floor', function()
        local r = use({
            spellEffect = EFFECT_BIND, targetSpeed = 40,
            durationPreResist = 3, resistRate = 1, addStatusOK = true,
        })
        assert(r.power == 40 and r.duration == 5)
    end)

    it('addle sub-power ladder', function()
        local r = use({
            spellEffect = EFFECT_ADDLE, casterStat = 150, targetStat = 50,
            addStatusOK = true, resistRate = 1,
        })
        assert(r.subPower == 40)
    end)

    it('dispel erase vs no effect', function()
        local r = use({
            spellEffect = EFFECT_NONE, dispelAttempted = true, dispelledEffect = EFFECT_POISON,
        })
        assert(r.msg == MSG_MAGIC_ERASE and r.returnEffect == EFFECT_POISON)
        r = use({ spellEffect = 0, dispelAttempted = true, dispelledEffect = 0 })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('magic burst message offset 1', function()
        local r = use({
            spellEffect = EFFECT_POISON, messageOffset = 1, skillchainCount = 1,
            addStatusOK = true, resistRate = 1,
        })
        assert(r.msg == MSG_MAGIC_BURST_ENFEEB and r.triggerRoeBurst == true)
    end)

    it('addStatus fail no effect', function()
        local r = use({ spellEffect = EFFECT_POISON, addStatusOK = false, resistRate = 1, potency = 5 })
        assert(r.applyEffect ~= true and r.msg == MSG_MAGIC_NO_EFFECT)
    end)
end)
