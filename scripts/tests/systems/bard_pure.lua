-----------------------------------
-- Pure system tests for Bard dual-wire (slice 6741).
-- Calls production xi.job_utils.bard pure exports.
-- Goldens match internal/bard (0898).
-----------------------------------

require('scripts/globals/job_utils/bard')

local b = xi.job_utils.bard

describe('Bard pure pins', function()
    it('bases and fixed params', function()
        assert(b.soulVoicePower == 1 and b.soulVoiceDuration == 180)
        assert(b.pianissimoPower == 0 and b.pianissimoDuration == 60)
        assert(b.nightingaleDuration == 60 and b.troubadourDuration == 60)
        assert(b.tenutoDuration == 60)
        assert(b.marcatoPower == 50 and b.marcatoDuration == 60)
        assert(b.clarionCallPower == 10 and b.clarionCallDuration == 180)
        assert(b.oneHourRecastSecondsPerMod == 60)

        local p = b.soulVoiceFromParams()
        assert(p.power == 1 and p.duration == 180)
        p = b.marcatoFromParams()
        assert(p.power == 50 and p.duration == 60)
        p = b.clarionCallFromParams()
        assert(p.power == 10 and p.duration == 180)
        p = b.pianissimoFromParams()
        assert(p.duration == 60)
    end)
end)

describe('oneHour recast', function()
    it('recast', function()
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(b.oneHourRecastFromParams({ abilityRecast = 60, oneHourRecastMod = 5 }) == 0)
        assert(b.oneHourRecastFromParams({ abilityRecast = 0, oneHourRecastMod = 1 }) == 0)
    end)
end)
