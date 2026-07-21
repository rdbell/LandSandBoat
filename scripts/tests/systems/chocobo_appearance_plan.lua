-----------------------------------
-- Pure system tests for chocobo appearance + report flag (slice 6160).
-----------------------------------

describe('chocobo appearance pure plan', function()
    local EGG, CHICK, ADOLESCENT, ADULT_1 = 1, 2, 3, 4
    local YELLOW = 0

    local function hasReport(eventCount)
        if eventCount > 0 then
            return 0xFFFFFFFF
        end
        return 0
    end

    local function planAppearance(p)
        if p.stage == EGG then
            return { color = YELLOW, crest = 0, feet = 0, tail = 0, stage = p.stage, sex = 0 }
        end
        if p.stage < ADOLESCENT then
            return { color = YELLOW, crest = 0, feet = 0, tail = 0, stage = p.stage, sex = p.sex }
        end
        if p.stage < ADULT_1 then
            return { color = p.color, crest = 0, feet = 0, tail = 0, stage = p.stage, sex = p.sex }
        end
        local crest = p.discernment >= 128 and 1 or 0
        local feet  = p.strength >= 128 and 1 or 0
        local tail  = p.endurance >= 128 and 1 or 0
        return {
            color = p.color, crest = crest, feet = feet, tail = tail,
            stage = p.stage, sex = p.sex,
        }
    end

    -- Intro report packing from INTRO_MENU_PT_1
    local function packReport(eventStart, eventEnd, moreEvents)
        local report = bit.lshift(eventStart, 0) + bit.lshift(eventEnd, 20)
        if eventStart == eventEnd then
            report = report + 0x00000400
        else
            report = report + 0x00001000
        end
        if moreEvents then
            report = report + 0x80000000
        end
        return report
    end

    it('has report flag', function()
        assert(hasReport(0) == 0)
        assert(hasReport(1) == 0xFFFFFFFF)
        assert(hasReport(3) == 0xFFFFFFFF)
    end)

    it('egg and chick force yellow', function()
        local r = planAppearance({ stage = EGG, color = 3, sex = 1, strength = 200, endurance = 200, discernment = 200 })
        assert(r.color == YELLOW and r.sex == 0 and r.crest == 0)
        r = planAppearance({ stage = CHICK, color = 3, sex = 1, strength = 0, endurance = 0, discernment = 0 })
        assert(r.color == YELLOW and r.sex == 1)
    end)

    it('adolescent shows true color without adult traits', function()
        local r = planAppearance({ stage = ADOLESCENT, color = 2, sex = 0, strength = 200, endurance = 200, discernment = 200 })
        assert(r.color == 2 and r.crest == 0 and r.feet == 0 and r.tail == 0 and r.sex == 0)
    end)

    it('adult traits from stat thresholds', function()
        local r = planAppearance({
            stage = ADULT_1, color = 1, sex = 1,
            strength = 128, endurance = 127, discernment = 128,
        })
        assert(r.color == 1 and r.crest == 1 and r.feet == 1 and r.tail == 0 and r.sex == 1)
        r = planAppearance({
            stage = ADULT_1, color = 1, sex = 0,
            strength = 127, endurance = 128, discernment = 127,
        })
        assert(r.crest == 0 and r.feet == 0 and r.tail == 1)
    end)

    it('packs intro report bits', function()
        local same = packReport(5, 5, false)
        assert(bit.band(same, 0x00000400) ~= 0)
        assert(bit.band(same, 0x00001000) == 0)
        assert(bit.band(same, 0x3FF) == 5)
        local span = packReport(5, 8, true)
        assert(bit.band(span, 0x00001000) ~= 0)
        assert(bit.band(span, 0x80000000) ~= 0)
        assert(bit.band(span, 0x3FF) == 5)
        -- end in bits 20..30 (bit 31 is moreEvents)
        assert(bit.band(bit.rshift(span, 20), 0x7FF) == 8)
    end)
end)
