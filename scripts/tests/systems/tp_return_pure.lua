-----------------------------------
-- Pure system tests for combat TP return dual-wire helpers (slice 6679).
-- Calls production xi.combat.tp pure exports (not local copies).
-- Goldens match internal/attackutils CalculateTPReturn (0801).
-----------------------------------

require('scripts/globals/combat/tp')

describe('TP return classification pure', function()
    it('isCharmedPCPet requires mob + charmed + PC master', function()
        assert(not xi.combat.tp.isCharmedPCPet(false, true, true))
        assert(not xi.combat.tp.isCharmedPCPet(true, false, true))
        assert(not xi.combat.tp.isCharmedPCPet(true, true, false))
        assert(xi.combat.tp.isCharmedPCPet(true, true, true))
    end)

    it('usePCOrPetTPFormula is true for non-mobs and charmed PC pets', function()
        assert(xi.combat.tp.usePCOrPetTPFormula(false, false))
        assert(xi.combat.tp.usePCOrPetTPFormula(false, true))
        assert(not xi.combat.tp.usePCOrPetTPFormula(true, false))
        assert(xi.combat.tp.usePCOrPetTPFormula(true, true))
    end)
end)

describe('TP return PC/pet delay bands', function()
    local function tp(delay)
        return xi.combat.tp.calculateTPReturnFromDelay(true, delay)
    end

    it('pins exact floors at band edges and interiors', function()
        -- ≤180: 61 + (delay-180)*63/360
        assert(tp(0) == 29)
        assert(tp(96) == 46)
        assert(tp(180) == 61)

        -- >180: 61 + (delay-180)*88/360
        assert(tp(181) == 61)
        assert(tp(240) == 75)
        assert(tp(360) == 105)
        assert(tp(450) == 127)
        assert(tp(480) == 134)
        assert(tp(540) == 149)

        -- >540: 149 + (delay-540)*20/360
        assert(tp(541) == 149)
        assert(tp(630) == 154)

        -- >630: 154 + (delay-630)*28/360
        assert(tp(631) == 154)
        assert(tp(720) == 161)

        -- >720: 161 + (delay-720)*24/360
        assert(tp(721) == 161)
        assert(tp(900) == 173)

        -- >900: 173 + (delay-900)*28/360
        assert(tp(901) == 173)
        assert(tp(1080) == 187)
        assert(tp(1440) == 215)
    end)

    it('uses strict > thresholds (value at threshold stays lower band)', function()
        -- At 180 uses ≤180 arm; at 181 uses >180 arm (both floor to 61).
        assert(tp(180) == 61)
        assert(tp(181) == 61)
        assert(tp(540) == 149)
        assert(tp(541) == 149)
        assert(tp(720) == 161)
        assert(tp(721) == 161)
        assert(tp(900) == 173)
        assert(tp(901) == 173)
    end)
end)

describe('TP return mob delay bands', function()
    local function tp(delay)
        return xi.combat.tp.calculateTPReturnFromDelay(false, delay)
    end

    it('pins exact floors at band edges and interiors', function()
        -- ≤180: 50 + (delay-180)*15/180
        assert(tp(0) == 35)
        assert(tp(96) == 43)
        assert(tp(180) == 50)

        -- >180: 50 + (delay-180)*65/270
        assert(tp(181) == 50)
        assert(tp(240) == 64)
        assert(tp(360) == 93)
        assert(tp(450) == 115)

        -- >450: 115 + (delay-450)*15/30
        assert(tp(451) == 115)
        assert(tp(480) == 130)

        -- >480: 130 + (delay-480)*15/30
        assert(tp(481) == 130)
        assert(tp(530) == 155) -- last tick of this band

        -- >530: 145 + (delay-530)*35/470 — LSB discontinuity 530→531
        assert(tp(531) == 145)
        assert(tp(540) == 145)
        assert(tp(630) == 152)
        assert(tp(720) == 159)
        assert(tp(900) == 172)
        assert(tp(1080) == 185)
        assert(tp(1440) == 212)
    end)

    it('documents the mob 530→531 discontinuity', function()
        assert(tp(530) == 155)
        assert(tp(531) == 145)
    end)
end)

describe('TP return PC vs mob independence', function()
    it('selects distinct tables at the same delay', function()
        local pc = xi.combat.tp.calculateTPReturnFromDelay(true, 240)
        local mob = xi.combat.tp.calculateTPReturnFromDelay(false, 240)
        assert(pc == 75)
        assert(mob == 64)
        assert(pc ~= mob)
    end)
end)
