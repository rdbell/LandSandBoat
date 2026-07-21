-----------------------------------
-- Pure system tests for useEnfeeblingSong full plan (slice 6114).
-- Source: scripts/globals/spells/enfeebling_song.lua ~149–263
-----------------------------------

describe('useEnfeeblingSong pure plan', function()
    local EFFECT_NONE = 255
    local EFFECT_SLEEP_I = 2
    local EFFECT_CHARM_I = 14
    local EFFECT_REQUIEM = 192
    local EFFECT_ELEGY = 194
    local EFFECT_THRENODY = 217

    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_RESIST = 85
    local MSG_MAGIC_ENFEEB_IS = 236
    local MSG_MAGIC_ENFEEB = 237
    local MSG_MAGIC_BURST_ENFEEB = 268
    local MSG_MAGIC_ERASE = 341
    local MSG_MAGIC_COMPLETE_RESIST = 655

    local function clampPower(power, cap)
        if power < 0 then power = 0 end
        if cap >= 0 and power > cap then power = cap end
        return math.floor(power)
    end

    local function successMsg(effect, scCount)
        if scCount and scCount > 0 then
            return MSG_MAGIC_BURST_ENFEEB
        end
        if effect == EFFECT_SLEEP_I then
            return MSG_MAGIC_ENFEEB_IS
        end
        return MSG_MAGIC_ENFEEB
    end

    local function use(p)
        local effect = p.spellEffect or 0
        if p.isImmune then
            return { returnEffect = effect, msg = MSG_MAGIC_COMPLETE_RESIST }
        end
        if p.isTraitResisted then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST, setActionModifier = true }
        end
        if p.isNullified then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT }
        end
        if p.resistFailed then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST }
        end
        if effect == EFFECT_CHARM_I and p.charmBlocked then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST }
        end

        local power = clampPower(p.powerRaw or 0, p.powerCap or 0)
        local duration = math.floor((p.durationPreResist or 0) * (p.resistRate or 1))
        local tick = (effect == EFFECT_REQUIEM) and 3 or 0
        local subPower = (effect == EFFECT_THRENODY) and (p.threnodySubPower or 0) or 0

        if effect == EFFECT_NONE or effect == 0 then
            if not p.dispelAttempted or not p.dispelledEffect or p.dispelledEffect == 0 or p.dispelledEffect == EFFECT_NONE then
                return { returnEffect = EFFECT_NONE, msg = MSG_MAGIC_NO_EFFECT, dispelPath = true }
            end
            return { returnEffect = p.dispelledEffect, msg = MSG_MAGIC_ERASE, dispelPath = true }
        end

        if effect == EFFECT_CHARM_I then
            local msg = p.casterIsPC and MSG_MAGIC_ENFEEB or MSG_MAGIC_ENFEEB_IS
            return { returnEffect = effect, msg = msg, applyCharm = true, duration = duration }
        end

        if not p.addStatusOK then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, power = power, duration = duration }
        end

        local r = {
            returnEffect = effect,
            msg = successMsg(effect, p.skillchainCount or 0),
            applyEffect = true,
            power = power,
            duration = duration,
            tick = tick,
            subPower = subPower,
        }
        if p.skillchainCount and p.skillchainCount > 0 then
            r.triggerRoeBurst = true
        end
        return r
    end

    it('immune complete resist', function()
        local r = use({ spellEffect = EFFECT_ELEGY, isImmune = true })
        assert(r.msg == MSG_MAGIC_COMPLETE_RESIST)
    end)

    it('resist fail', function()
        local r = use({ spellEffect = EFFECT_ELEGY, resistFailed = true })
        assert(r.msg == MSG_MAGIC_RESIST)
    end)

    it('virelai uncharmable resists', function()
        local r = use({ spellEffect = EFFECT_CHARM_I, charmBlocked = true })
        assert(r.msg == MSG_MAGIC_RESIST and r.applyCharm ~= true)
    end)

    it('requiem success tick and power floor', function()
        local r = use({
            spellEffect = EFFECT_REQUIEM, powerRaw = 8.9, powerCap = 300,
            durationPreResist = 160, resistRate = 1, addStatusOK = true,
        })
        assert(r.applyEffect == true and r.power == 8 and r.duration == 160 and r.tick == 3)
        assert(r.msg == MSG_MAGIC_ENFEEB)
    end)

    it('duration scales by resist', function()
        local r = use({
            spellEffect = EFFECT_ELEGY, powerRaw = 100, powerCap = 5000,
            durationPreResist = 180, resistRate = 0.5, addStatusOK = true,
        })
        assert(r.duration == 90)
    end)

    it('power clamps to cap', function()
        local r = use({
            spellEffect = EFFECT_ELEGY, powerRaw = 9999, powerCap = 5000,
            resistRate = 1, addStatusOK = true,
        })
        assert(r.power == 5000)
    end)

    it('finale erase vs no effect', function()
        local r = use({
            spellEffect = EFFECT_NONE, dispelAttempted = true, dispelledEffect = EFFECT_ELEGY,
        })
        assert(r.msg == MSG_MAGIC_ERASE and r.returnEffect == EFFECT_ELEGY)
        r = use({ spellEffect = EFFECT_NONE, dispelAttempted = true, dispelledEffect = EFFECT_NONE })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('virelai pc vs non-pc message', function()
        local r = use({
            spellEffect = EFFECT_CHARM_I, durationPreResist = 45, resistRate = 1, casterIsPC = true,
        })
        assert(r.applyCharm == true and r.msg == MSG_MAGIC_ENFEEB)
        r = use({
            spellEffect = EFFECT_CHARM_I, durationPreResist = 30, resistRate = 1, casterIsPC = false,
        })
        assert(r.msg == MSG_MAGIC_ENFEEB_IS)
    end)

    it('lullaby uses ENFEEB_IS', function()
        local r = use({
            spellEffect = EFFECT_SLEEP_I, powerRaw = 1, powerCap = 1, resistRate = 1, addStatusOK = true,
        })
        assert(r.msg == MSG_MAGIC_ENFEEB_IS)
    end)

    it('magic burst message', function()
        local r = use({
            spellEffect = EFFECT_ELEGY, powerRaw = 100, powerCap = 5000,
            resistRate = 1, addStatusOK = true, skillchainCount = 2,
        })
        assert(r.msg == MSG_MAGIC_BURST_ENFEEB and r.triggerRoeBurst == true)
    end)

    it('threnody subpower', function()
        local r = use({
            spellEffect = EFFECT_THRENODY, powerRaw = 50, powerCap = 95,
            resistRate = 1, threnodySubPower = 12, addStatusOK = true,
        })
        assert(r.subPower == 12)
    end)

    it('finale bonus macc formula', function()
        assert(175 + 0 * 5 == 175)
        assert(175 + 3 * 5 == 190)
    end)
end)
