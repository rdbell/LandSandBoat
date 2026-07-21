-----------------------------------
-- Pure system tests for blue useEnfeeblingSpell plan (slice 6125).
-- Source: scripts/globals/bluemagic.lua ~697–761
-----------------------------------

describe('blue useEnfeeblingSpell pure plan', function()
    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_RESIST = 85
    local MSG_MAGIC_ENFEEB_IS = 236
    local MSG_MAGIC_BURST_ENFEEB_IS = 271
    local MSG_MAGIC_COMPLETE_RESIST = 655
    local ACTION_MODIFIER_RESIST = 1

    local function use(p)
        local effect = p.effect or 0
        if p.isConal and not p.targetInFront then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, setMsg = true }
        end
        if p.isGaze and (not p.targetFacingCaster or not p.casterFacingTarget) then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, setMsg = true }
        end
        if p.isImmune then
            return { returnEffect = effect, msg = MSG_MAGIC_COMPLETE_RESIST, setMsg = true }
        end
        if p.isTraitResisted then
            return {
                returnEffect = effect, msg = MSG_MAGIC_RESIST, setMsg = true,
                setActionModifier = true, actionModifier = ACTION_MODIFIER_RESIST,
            }
        end
        if p.isNullified then
            return { returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, setMsg = true }
        end
        if p.resistFailed then
            return { returnEffect = effect, msg = MSG_MAGIC_RESIST, setMsg = true }
        end

        local duration = math.floor((p.duration or 0) * (p.resistRate or 1))
        if not p.addStatusOK then
            return {
                returnEffect = effect, msg = MSG_MAGIC_NO_EFFECT, setMsg = true,
                power = p.power or 0, duration = duration, tick = p.tick or 0, tier = p.tier or 0,
            }
        end
        local msg = MSG_MAGIC_ENFEEB_IS
        local triggerRoe = false
        if (p.skillchainCount or 0) > 0 then
            msg = MSG_MAGIC_BURST_ENFEEB_IS
            triggerRoe = true
        end
        return {
            returnEffect = effect, msg = msg, setMsg = true,
            applyEffect = true,
            power = p.power or 0, duration = duration, tick = p.tick or 0, tier = p.tier or 0,
            triggerRoeBurst = triggerRoe,
        }
    end

    it('conal miss MAGIC_NO_EFFECT', function()
        local r = use({ effect = 10, isConal = true, targetInFront = false })
        assert(r.msg == MSG_MAGIC_NO_EFFECT and r.applyEffect ~= true)
    end)

    it('gaze miss MAGIC_NO_EFFECT', function()
        local r = use({
            effect = 10, isGaze = true,
            targetFacingCaster = true, casterFacingTarget = false,
        })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('immune complete resist', function()
        local r = use({ effect = 4, isImmune = true })
        assert(r.msg == MSG_MAGIC_COMPLETE_RESIST)
    end)

    it('trait resist sets modifier', function()
        local r = use({ effect = 4, isTraitResisted = true })
        assert(r.msg == MSG_MAGIC_RESIST and r.setActionModifier == true)
        assert(r.actionModifier == ACTION_MODIFIER_RESIST)
    end)

    it('nullified MAGIC_NO_EFFECT', function()
        local r = use({ effect = 4, isNullified = true })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('resist fail MAGIC_RESIST', function()
        local r = use({ effect = 4, resistFailed = true })
        assert(r.msg == MSG_MAGIC_RESIST)
    end)

    it('success scales duration and MAGIC_ENFEEB_IS', function()
        local r = use({
            effect = 4, power = 25, duration = 60, resistRate = 0.5,
            tick = 3, tier = 1, addStatusOK = true,
        })
        assert(r.applyEffect == true and r.duration == 30 and r.power == 25)
        assert(r.msg == MSG_MAGIC_ENFEEB_IS and r.tick == 3 and r.tier == 1)
    end)

    it('skillchain burst message and roe', function()
        local r = use({
            effect = 4, power = 10, duration = 30, resistRate = 1,
            addStatusOK = true, skillchainCount = 2,
        })
        assert(r.msg == MSG_MAGIC_BURST_ENFEEB_IS and r.triggerRoeBurst == true)
    end)

    it('addStatus fail MAGIC_NO_EFFECT keeps plan fields', function()
        local r = use({
            effect = 4, power = 15, duration = 40, resistRate = 1, addStatusOK = false,
        })
        assert(r.msg == MSG_MAGIC_NO_EFFECT and r.applyEffect ~= true)
        assert(r.power == 15 and r.duration == 40)
    end)

    it('conal ok when in front continues', function()
        local r = use({
            effect = 4, isConal = true, targetInFront = true,
            power = 5, duration = 20, resistRate = 1, addStatusOK = true,
        })
        assert(r.applyEffect == true and r.msg == MSG_MAGIC_ENFEEB_IS)
    end)
end)
