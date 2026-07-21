-----------------------------------
-- Pure system tests for useEnhancingNinjutsu full plan (slice 6116).
-- Source: scripts/globals/spells/enhancing_ninjutsu.lua ~66–132
-----------------------------------

describe('useEnhancingNinjutsu pure plan', function()
    local EFFECT_COPY_IMAGE = 66
    local EFFECT_THIRD_EYE = 67
    local EFFECT_INVISIBLE = 69
    local EFFECT_SNEAK = 71
    local EFFECT_PAX = 171
    local EFFECT_ENMITY_BOOST = 289
    local EFFECT_STORE_TP = 227
    local EFFECT_MIGAWARI = 471

    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_GAIN_EFFECT = 230

    local UTSUSEMI_DURATION = 900

    local function use(p)
        local effect = p.spellEffect or 0
        local duration = p.duration or 0
        local alwaysOverwrite = p.alwaysOverwrite == true
        local power = p.power or 0
        local subPower = p.subPower or 0
        local tablePower = p.tablePower or 0
        local paramThree = 0

        local delPax = false
        local delEnmityBoost = false
        local delThirdEye = false
        local delSpellEffect = false

        if effect == EFFECT_ENMITY_BOOST then
            delPax = true
        elseif effect == EFFECT_SNEAK or effect == EFFECT_INVISIBLE then
            paramThree = 10
        elseif effect == EFFECT_PAX then
            delEnmityBoost = true
        end

        if alwaysOverwrite then
            delSpellEffect = true
            return {
                returnEffect = effect,
                applyEffect = true,
                power = power,
                duration = duration,
                tick = paramThree,
                subPower = subPower,
                delPax = delPax,
                delEnmityBoost = delEnmityBoost,
                delSpellEffect = delSpellEffect,
                setMsg = false,
            }
        end

        if effect == EFFECT_COPY_IMAGE then
            if p.hasThirdEye then
                delThirdEye = true
            end
            paramThree = tablePower - 2
            if not p.hasCopyImage or (p.existingCopyImagePower or 0) <= paramThree then
                return {
                    returnEffect = effect,
                    applyEffect = true,
                    power = paramThree,
                    duration = UTSUSEMI_DURATION,
                    tick = duration,
                    icon = subPower,
                    subPower = power,
                    delThirdEye = delThirdEye,
                    setMsg = true,
                    msg = MSG_MAGIC_GAIN_EFFECT,
                }
            end
            return {
                returnEffect = effect,
                applyEffect = false,
                power = paramThree,
                duration = UTSUSEMI_DURATION,
                tick = duration,
                icon = subPower,
                subPower = power,
                delThirdEye = delThirdEye,
                setMsg = true,
                msg = MSG_MAGIC_NO_EFFECT,
            }
        end

        -- default addStatusEffect path
        if p.addStatusOK then
            return {
                returnEffect = effect,
                applyEffect = true,
                power = power,
                duration = duration,
                tick = paramThree,
                subPower = subPower,
                delPax = delPax,
                delEnmityBoost = delEnmityBoost,
                setMsg = true,
                msg = MSG_MAGIC_GAIN_EFFECT,
            }
        end
        return {
            returnEffect = effect,
            applyEffect = false,
            power = power,
            duration = duration,
            tick = paramThree,
            subPower = subPower,
            delPax = delPax,
            delEnmityBoost = delEnmityBoost,
            setMsg = true,
            msg = MSG_MAGIC_NO_EFFECT,
        }
    end

    it('alwaysOverwrite gekka deletes pax and applies without setMsg', function()
        local r = use({
            spellEffect = EFFECT_ENMITY_BOOST, duration = 300, alwaysOverwrite = true,
            power = 30, subPower = 0,
        })
        assert(r.delPax == true and r.delSpellEffect == true and r.applyEffect == true)
        assert(r.setMsg ~= true)
        assert(r.power == 30 and r.duration == 300 and r.tick == 0)
    end)

    it('alwaysOverwrite yain deletes enmity boost', function()
        local r = use({
            spellEffect = EFFECT_PAX, duration = 300, alwaysOverwrite = true,
            power = 15,
        })
        assert(r.delEnmityBoost == true and r.delSpellEffect == true)
    end)

    it('migawari always overwrite applies power/subPower', function()
        local r = use({
            spellEffect = EFFECT_MIGAWARI, duration = 60, alwaysOverwrite = true,
            power = 50, subPower = 100,
        })
        assert(r.applyEffect == true and r.power == 50 and r.subPower == 100)
        assert(r.setMsg ~= true)
    end)

    it('tonko sets tick 10 and gain on success', function()
        local r = use({
            spellEffect = EFFECT_INVISIBLE, duration = 600, alwaysOverwrite = false,
            power = 0, addStatusOK = true,
        })
        assert(r.tick == 10 and r.msg == MSG_MAGIC_GAIN_EFFECT and r.applyEffect == true)
    end)

    it('monomi fail sets MAGIC_NO_EFFECT', function()
        local r = use({
            spellEffect = EFFECT_SNEAK, duration = 420, alwaysOverwrite = false,
            power = 0, addStatusOK = false,
        })
        assert(r.tick == 10 and r.msg == MSG_MAGIC_NO_EFFECT and r.applyEffect == false)
    end)

    it('utsusemi ichi applies when no existing', function()
        -- table power 3 → paramThree 1; calculated power 3 → subPower; icon=subPower(445)
        local r = use({
            spellEffect = EFFECT_COPY_IMAGE, duration = 0, alwaysOverwrite = false,
            tablePower = 3, power = 3, subPower = 445, hasCopyImage = false,
        })
        assert(r.applyEffect == true and r.msg == MSG_MAGIC_GAIN_EFFECT)
        assert(r.power == 1 and r.duration == 900 and r.tick == 0)
        assert(r.icon == 445 and r.subPower == 3)
        assert(r.delThirdEye ~= true)
    end)

    it('utsusemi san replaces weaker existing and strips third eye', function()
        -- table 5 → paramThree 3; existing power 2 <= 3 → apply
        local r = use({
            spellEffect = EFFECT_COPY_IMAGE, duration = 0, alwaysOverwrite = false,
            tablePower = 5, power = 6, subPower = 446,
            hasCopyImage = true, existingCopyImagePower = 2, hasThirdEye = true,
        })
        assert(r.delThirdEye == true and r.applyEffect == true)
        assert(r.power == 3 and r.subPower == 6 and r.icon == 446)
        assert(r.msg == MSG_MAGIC_GAIN_EFFECT)
    end)

    it('utsusemi fails when existing stronger', function()
        -- table 3 → paramThree 1; existing 3 > 1 → no effect
        local r = use({
            spellEffect = EFFECT_COPY_IMAGE, duration = 0,
            tablePower = 3, power = 3, subPower = 445,
            hasCopyImage = true, existingCopyImagePower = 3, hasThirdEye = true,
        })
        assert(r.applyEffect == false and r.msg == MSG_MAGIC_NO_EFFECT)
        assert(r.delThirdEye == true) -- still strips third eye before check
        assert(r.power == 1)
    end)

    it('utsusemi equal power replaces (existing <= paramThree)', function()
        local r = use({
            spellEffect = EFFECT_COPY_IMAGE, duration = 0,
            tablePower = 4, power = 4, subPower = 446,
            hasCopyImage = true, existingCopyImagePower = 2,
        })
        assert(r.power == 2 and r.applyEffect == true)
    end)

    it('store tp always overwrite no special del', function()
        local r = use({
            spellEffect = EFFECT_STORE_TP, duration = 180, alwaysOverwrite = true,
            power = 10,
        })
        assert(r.delPax ~= true and r.delEnmityBoost ~= true and r.delSpellEffect == true)
        assert(r.applyEffect == true and r.setMsg ~= true)
    end)
end)
