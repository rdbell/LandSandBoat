-----------------------------------
-- Pure system tests for xi.ability.adjustDamage late inject half (slice 6097).
-- Mirrors scripts/globals/ability.lua ~54–99 after AdjustEarly.
-----------------------------------

describe('ability adjustDamage late pure injects', function()
    local ATTACK_PHYSICAL = 1
    local ATTACK_MAGICAL  = 2
    local ATTACK_RANGED   = 3
    local ATTACK_BREATH   = 4
    local MSG_ANTICIPATE  = 30

    local function clampElement(skillparam)
        local el = skillparam - 5
        if el < 0 then
            return 0
        end
        if el > 8 then
            return 8
        end
        return el
    end

    ---@param p table inject fields
    ---@return number damage, number|nil setMsg, boolean wake, boolean enmity
    local function adjustLate(p)
        local skilltype = p.skilltype or 0

        -- Third Eye anticipate
        if
            (skilltype == ATTACK_PHYSICAL or skilltype == ATTACK_RANGED) and
            p.anticipated
        then
            return 0, MSG_ANTICIPATE, false, false
        end

        local dmg = p.dmg or 0

        -- Skilltype dmg-taken product (pre-resolved injects)
        if skilltype == ATTACK_PHYSICAL then
            dmg = p.physicalDmgTaken or dmg
        elseif skilltype == ATTACK_MAGICAL then
            dmg = math.floor(dmg * (p.magicAdj or 1))
            dmg = math.floor(dmg * (p.absorption or 1))
            dmg = math.floor(dmg * (p.nullification or 1))
            dmg = math.floor(p.severeDamage or dmg)
        elseif skilltype == ATTACK_BREATH then
            dmg = math.floor(dmg * (p.breathAdj or 1))
            dmg = math.floor(dmg * (p.absorption or 1))
            dmg = math.floor(dmg * (p.nullification or 1))
            dmg = math.floor(p.severeDamage or dmg)
        elseif skilltype == ATTACK_RANGED then
            dmg = p.rangedDmgTaken or dmg
        end

        if dmg < 0 then
            return dmg, nil, false, false
        end

        dmg = p.phalanx or dmg -- inject after handlePhalanx

        if skilltype == ATTACK_MAGICAL then
            dmg = p.oneForAll or dmg
        end

        dmg = p.stoneskin or dmg

        local wake = dmg > 0
        local enmity = dmg > 0
        return dmg, nil, wake, enmity
    end

    it('element clamp from skillparam', function()
        assert(clampElement(5) == 0)   -- NONE
        assert(clampElement(6) == 1)   -- FIRE
        assert(clampElement(13) == 8)  -- DARK
        assert(clampElement(20) == 8)  -- clamp high
        assert(clampElement(0) == 0)   -- clamp low
    end)

    it('physical/ranged anticipate short-circuits', function()
        local dmg, msg = adjustLate({ skilltype = ATTACK_PHYSICAL, dmg = 100, anticipated = true })
        assert(dmg == 0 and msg == MSG_ANTICIPATE)
        dmg, msg = adjustLate({ skilltype = ATTACK_RANGED, dmg = 100, anticipated = true })
        assert(dmg == 0 and msg == MSG_ANTICIPATE)
        dmg, msg = adjustLate({ skilltype = ATTACK_MAGICAL, dmg = 100, anticipated = true })
        assert(dmg == 100 and msg == nil) -- magical ignores anticipate gate
    end)

    it('magical floor product chain', function()
        -- 100 * 0.9 = 90; *1.1 absorb = 99; *1 null = 99; severe inject 80
        local dmg, _, wake, enmity = adjustLate({
            skilltype = ATTACK_MAGICAL,
            dmg = 100,
            magicAdj = 0.9,
            absorption = 1.1,
            nullification = 1,
            severeDamage = 80,
            phalanx = 80,
            oneForAll = 75,
            stoneskin = 70,
        })
        assert(dmg == 70, 'got ' .. tostring(dmg))
        assert(wake and enmity)
    end)

    it('negative dmg skips phalanx/stoneskin/wake', function()
        local dmg, _, wake, enmity = adjustLate({
            skilltype = ATTACK_MAGICAL,
            dmg = 50,
            magicAdj = 1,
            absorption = -1, -- floor(50*-1) = -50
            nullification = 1,
            severeDamage = -50,
        })
        assert(dmg == -50)
        assert(not wake and not enmity)
    end)

    it('physical uses physicalDmgTaken inject then phalanx/stoneskin', function()
        local dmg, _, wake = adjustLate({
            skilltype = ATTACK_PHYSICAL,
            dmg = 200,
            physicalDmgTaken = 150,
            phalanx = 140,
            stoneskin = 120,
        })
        assert(dmg == 120)
        assert(wake)
    end)

    it('zero damage does not wake', function()
        local dmg, _, wake, enmity = adjustLate({
            skilltype = ATTACK_PHYSICAL,
            dmg = 50,
            physicalDmgTaken = 10,
            phalanx = 0,
            stoneskin = 0,
        })
        assert(dmg == 0)
        assert(not wake and not enmity)
    end)
end)
