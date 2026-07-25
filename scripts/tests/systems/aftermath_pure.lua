-----------------------------------
-- Pure system tests for aftermath dual-wire (slice 6726).
-- Calls production xi.aftermath pure exports.
-- Goldens match internal/aftermath (0878 / 6134).
-----------------------------------

require('scripts/globals/aftermath')

local a = xi.aftermath

describe('Aftermath type and pins', function()
    it('type and effect constants', function()
        assert(a.type.RELIC == 1 and a.type.MYTHIC == 2 and a.type.EMPYREAN == 3)
        assert(a.effectAftermath == 273)
        assert(a.effectAftermathLV1 == 270)
        assert(a.effectAftermathLV2 == 271)
        assert(a.effectAftermathLV3 == 272)
    end)
end)

describe('levelFromTP / levelIcon', function()
    it('floor tp/1000 and icon map', function()
        assert(a.levelFromTP(0) == 0)
        assert(a.levelFromTP(999) == 0)
        assert(a.levelFromTP(1000) == 1)
        assert(a.levelFromTP(1999) == 1)
        assert(a.levelFromTP(2000) == 2)
        assert(a.levelFromTP(3000) == 3)
        assert(a.levelIcon(1) == 270)
        assert(a.levelIcon(2) == 271)
        assert(a.levelIcon(3) == 272)
        assert(a.levelIcon(0) == 273)
    end)
end)

describe('validID includePets clearsSpikes', function()
    it('id ranges and host hints', function()
        assert(a.validID(a.type.RELIC, 1) and a.validID(a.type.RELIC, 28))
        assert(not a.validID(a.type.RELIC, 0) and not a.validID(a.type.RELIC, 29))
        assert(a.validID(a.type.MYTHIC, 29) and a.validID(a.type.MYTHIC, 43))
        assert(not a.validID(a.type.MYTHIC, 28) and not a.validID(a.type.MYTHIC, 44))
        assert(a.validID(a.type.EMPYREAN, 44) and a.validID(a.type.EMPYREAN, 45))
        assert(not a.validID(a.type.EMPYREAN, 43) and not a.validID(a.type.EMPYREAN, 46))
        assert(a.includePets(19) and not a.includePets(1))
        assert(a.clearsSpikes(8) and a.clearsSpikes(22) and not a.clearsSpikes(1))
    end)
end)

describe('relic / durationSecondsFromParams', function()
    it('relic tier formulas', function()
        assert(a.getTier1RelicDuration(2000) == 40)
        assert(a.getTier2RelicDuration(2000) == 120)
        assert(a.relicDurationSeconds(1, 1000) == 20)
        assert(a.relicDurationSeconds(14, 3000) == 60)
        assert(a.relicDurationSeconds(15, 1000) == 60)
        assert(a.relicDurationSeconds(28, 2000) == 120)
        assert(a.relicDurationSeconds(0, 1000) == nil)
        assert(a.relicDurationSeconds(29, 1000) == nil)
    end)

    it('mythic and empyrean duration tables', function()
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 29, tp = 1000 }) == 60)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 29, tp = 2000 }) == 90)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 29, tp = 3000 }) == 120)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 30, tp = 1000 }) == 180)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 30, tp = 2000 }) == 90)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.EMPYREAN, id = 44, tp = 1000 }) == 30)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.EMPYREAN, id = 44, tp = 3000 }) == 90)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.EMPYREAN, id = 45, tp = 2000 }) == 120)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.RELIC, id = 8, tp = 1000 }) == 20)
        assert(a.durationSecondsFromParams({ aftermathType = a.type.MYTHIC, id = 29, tp = 500 }) == nil)
    end)
end)

describe('empyreanPowerFromParams', function()
    it('300/400/500 ladder', function()
        assert(a.empyreanPowerFromParams({ id = 44, level = 1 }) == 300)
        assert(a.empyreanPowerFromParams({ id = 44, level = 2 }) == 400)
        assert(a.empyreanPowerFromParams({ id = 45, level = 3 }) == 500)
        assert(a.empyreanPowerFromParams({ id = 44, level = 0 }) == nil)
        assert(a.empyreanPowerFromParams({ id = 29, level = 1 }) == nil)
    end)
end)

describe('canOverwriteFromParams', function()
    it('no effect and lower type gate', function()
        assert(a.canOverwriteFromParams({ hasEffect = false, newType = a.type.RELIC, newID = 1, newTP = 1000 }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.MYTHIC,
            newType = a.type.RELIC, newID = 1, newTP = 3000,
        }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.EMPYREAN,
            newType = a.type.MYTHIC, newID = 29, newTP = 3000,
        }))
    end)

    it('relic strict duration ms compare', function()
        assert(a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.RELIC,
            existingTimeRemainingMs = 39999,
            newType = a.type.RELIC, newID = 1, newTP = 2000,
        }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.RELIC,
            existingTimeRemainingMs = 40000,
            newType = a.type.RELIC, newID = 1, newTP = 2000,
        }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.RELIC,
            existingTimeRemainingMs = 40001,
            newType = a.type.RELIC, newID = 1, newTP = 2000,
        }))
    end)

    it('mythic empyrean level policy', function()
        assert(a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.MYTHIC,
            existingSubPower = 1500, newType = a.type.MYTHIC, newID = 29, newTP = 1000,
        }))
        assert(a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.MYTHIC,
            existingSubPower = 2500, newType = a.type.MYTHIC, newID = 29, newTP = 3000,
        }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.MYTHIC,
            existingSubPower = 2500, newType = a.type.MYTHIC, newID = 29, newTP = 2000,
        }))
        assert(not a.canOverwriteFromParams({
            hasEffect = true, existingTier = a.type.EMPYREAN,
            existingSubPower = 3000, newType = a.type.EMPYREAN, newID = 44, newTP = 1000,
        }))
    end)
end)

describe('addStatusEffectFromParams', function()
    it('gates non-pc no weapon invalid id', function()
        assert(not a.addStatusEffectFromParams({
            isPC = false, hasWeapon = true, aftermathID = 1, aftermathType = a.type.RELIC, tp = 1000,
        }).ok)
        assert(not a.addStatusEffectFromParams({
            isPC = true, hasWeapon = false, aftermathID = 1, aftermathType = a.type.RELIC, tp = 1000,
        }).ok)
        assert(not a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 29, aftermathType = a.type.RELIC, tp = 1000,
        }).ok)
    end)

    it('relic success and gungnir spikes', function()
        local r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 1, aftermathType = a.type.RELIC, tp = 2000,
        })
        assert(r.ok and r.delAftermath and not r.clearSpikes)
        assert(r.duration == 40 and r.power == 1 and r.subPower == 2000 and r.tier == 1)
        assert(not r.hasIcon)

        r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 8, aftermathType = a.type.RELIC, tp = 1000,
        })
        assert(r.ok and r.clearSpikes and r.duration == 20)

        r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 22, aftermathType = a.type.RELIC, tp = 1000,
        })
        assert(r.clearSpikes)
    end)

    it('mythic icons and durations', function()
        local r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 29, aftermathType = a.type.MYTHIC, tp = 1000,
        })
        assert(r.ok and r.duration == 60 and r.hasIcon and r.icon == 270)

        r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 29, aftermathType = a.type.MYTHIC, tp = 2500,
        })
        assert(r.duration == 90 and r.icon == 271)

        r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 29, aftermathType = a.type.MYTHIC, tp = 3000,
        })
        assert(r.duration == 120 and r.icon == 272)
    end)

    it('overwrite block lower type', function()
        local r = a.addStatusEffectFromParams({
            isPC = true, hasWeapon = true, aftermathID = 1, aftermathType = a.type.RELIC, tp = 2000,
            hasEffect = true, existingTier = a.type.MYTHIC, existingSubPower = 2000,
        })
        assert(not r.ok)
    end)
end)
