-----------------------------------
-- Pure system tests for checkAnticipated full inject (slice 6101).
-----------------------------------

describe('checkAnticipated pure injects', function()
    local function canRetain(isPC, isWeaponTwoHanded)
        return (not isPC) or isWeaponTwoHanded
    end

    local function retentionScaled(timeInEffectMs, retentionRateMod)
        local retentionLossPerMillisecond = 1 / 300
        local retentionModifier = utils.clamp(1 - retentionRateMod / 100, 0, 1)
        return utils.clamp(100 - timeInEffectMs * retentionLossPerMillisecond * retentionModifier, 0, 100) * 100
    end

    local function retains(scaledChance, roll)
        if scaledChance == 0 then
            return false
        end
        return roll <= scaledChance
    end

    local function checkAnticipated(p)
        if not p.hasThirdEye then
            return { anticipated = false, shouldDelete = false }
        end
        local can = canRetain(p.isPC, p.isWeaponTwoHanded)
        local scaled = 0
        if p.hasSeigan and can then
            scaled = retentionScaled(p.timeInEffectMs or 0, p.retentionRateMod or 0)
        end
        return {
            anticipated = true,
            shouldDelete = not retains(scaled, p.roll or 1),
        }
    end

    it('no Third Eye returns false', function()
        local r = checkAnticipated({ hasThirdEye = false, hasSeigan = true, isPC = true, isWeaponTwoHanded = true })
        assert(r.anticipated == false and r.shouldDelete == false)
    end)

    it('TE without Seigan always anticipates and deletes', function()
        local r = checkAnticipated({ hasThirdEye = true, hasSeigan = false, isPC = true, isWeaponTwoHanded = true, roll = 1 })
        assert(r.anticipated == true and r.shouldDelete == true)
    end)

    it('TE + Seigan + 2H PC retains on roll 1 at t=0', function()
        local r = checkAnticipated({
            hasThirdEye = true, hasSeigan = true, isPC = true, isWeaponTwoHanded = true,
            timeInEffectMs = 0, roll = 1,
        })
        assert(r.anticipated == true and r.shouldDelete == false)
    end)

    it('PC one-hand cannot retain', function()
        local r = checkAnticipated({
            hasThirdEye = true, hasSeigan = true, isPC = true, isWeaponTwoHanded = false, roll = 1,
        })
        assert(r.anticipated == true and r.shouldDelete == true)
    end)

    it('non-PC can retain without two-hand', function()
        local r = checkAnticipated({
            hasThirdEye = true, hasSeigan = true, isPC = false, isWeaponTwoHanded = false,
            timeInEffectMs = 0, roll = 1,
        })
        assert(r.anticipated == true and r.shouldDelete == false)
    end)
end)
