-----------------------------------
-- Pure system tests for Liement pack/check dual-wire (slice 6719).
-- Calls production xi.job_utils.rune_fencer pure exports.
-- Goldens match internal/liement (0823) and damage_affinity_capacity.h.
-----------------------------------

require('scripts/globals/job_utils/rune_fencer')

local run = xi.job_utils.rune_fencer

-- xi.damageType elemental ordinals used by Liement packing.
local dt =
{
    NONE    = 0,
    FIRE    = 6,
    ICE     = 7,
    WIND    = 8,
    EARTH   = 9,
    THUNDER = 10,
    WATER   = 11,
    LIGHT   = 12,
    DARK    = 13,
}

local function almost(a, b)
    return math.abs(a - b) < 1e-6
end

describe('Liement pure constants', function()
    it('pins base percent and pack bounds', function()
        assert(run.liementBaseAbsorbPercent == 85)
        assert(run.liementMaxPackedRunes == 4)
        assert(run.liementNibbleMask == 0xF)
    end)
end)

describe('packAbsorbTypes / unpackAbsorbTypes', function()
    it('packs low nibble first and caps at 4', function()
        local bits = run.packAbsorbTypes({ dt.FIRE, dt.ICE, dt.WIND, dt.EARTH })
        local want = dt.FIRE + bit.lshift(dt.ICE, 4) + bit.lshift(dt.WIND, 8) + bit.lshift(dt.EARTH, 12)
        assert(bits == want)

        local bits5 = run.packAbsorbTypes({ dt.FIRE, dt.ICE, dt.WIND, dt.EARTH, dt.THUNDER })
        assert(bits5 == bits)

        assert(run.packAbsorbTypes({}) == 0)
        assert(run.packAbsorbTypes({ 0x1F }) == 0xF)
    end)

    it('round-trips unpack', function()
        local inTypes = { dt.FIRE, dt.ICE, dt.WATER, dt.DARK }
        local out = run.unpackAbsorbTypes(run.packAbsorbTypes(inTypes))
        for i = 1, 4 do
            assert(out[i] == inTypes[i])
        end

        out = run.unpackAbsorbTypes(run.packAbsorbTypes({ dt.THUNDER }))
        assert(out[1] == dt.THUNDER and out[2] == 0 and out[3] == 0 and out[4] == 0)
    end)
end)

describe('checkLiementAbsorbFromParams', function()
    it('inactive and no-match return 1', function()
        local m, c = run.checkLiementAbsorbFromParams({
            active = false, absorbPower = 15,
            absorbTypeBits = run.packAbsorbTypes({ dt.FIRE }), damageType = dt.FIRE,
        })
        assert(almost(m, 1.0) and not c)

        m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 15,
            absorbTypeBits = run.packAbsorbTypes({ dt.FIRE, dt.ICE }), damageType = dt.THUNDER,
        })
        assert(almost(m, 1.0) and not c)
    end)

    it('retail power ladder for matching runes', function()
        -- 85+15*n → 100/115/130/145% absorb for 1..4 Fire nibbles
        for count, want in ipairs({ -1.00, -1.15, -1.30, -1.45 }) do
            local types = {}
            for i = 1, count do
                types[i] = dt.FIRE
            end

            local m, c = run.checkLiementAbsorbFromParams({
                active = true, absorbPower = 15,
                absorbTypeBits = run.packAbsorbTypes(types), damageType = dt.FIRE,
            })
            assert(c and almost(m, want), string.format('count=%d m=%s want=%s', count, tostring(m), tostring(want)))
        end
    end)

    it('counts only matching nibbles in mixed pack', function()
        local bits = run.packAbsorbTypes({ dt.FIRE, dt.ICE, dt.FIRE })
        local m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 15, absorbTypeBits = bits, damageType = dt.FIRE,
        })
        assert(c and almost(m, -1.15))

        m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 15, absorbTypeBits = bits, damageType = dt.ICE,
        })
        assert(c and almost(m, -1.0))

        m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 15, absorbTypeBits = bits, damageType = dt.WIND,
        })
        assert(not c and almost(m, 1.0))
    end)

    it('custom power and zero power still absorb', function()
        local m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 20,
            absorbTypeBits = run.packAbsorbTypes({ dt.WATER }), damageType = dt.WATER,
        })
        assert(c and almost(m, -1.05))

        m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 0,
            absorbTypeBits = run.packAbsorbTypes({ dt.LIGHT }), damageType = dt.LIGHT,
        })
        assert(c and almost(m, -0.85))
    end)

    it('empty bits do not absorb', function()
        local m, c = run.checkLiementAbsorbFromParams({
            active = true, absorbPower = 15, absorbTypeBits = 0, damageType = dt.FIRE,
        })
        assert(not c and almost(m, 1.0))
    end)

    it('each elemental type alone absorbs at 100%', function()
        for _, d in ipairs({ dt.FIRE, dt.ICE, dt.WIND, dt.EARTH, dt.THUNDER, dt.WATER, dt.LIGHT, dt.DARK }) do
            local m, c = run.checkLiementAbsorbFromParams({
                active = true, absorbPower = 15,
                absorbTypeBits = run.packAbsorbTypes({ d }), damageType = d,
            })
            assert(c and almost(m, -1.0), 'dt=' .. tostring(d))
        end
    end)
end)
