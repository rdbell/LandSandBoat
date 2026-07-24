require('scripts/globals/combat/physical_utilities')

describe('pDIF spike ratio PC arm', function()
    it('is zero outside the open (0.5, 1.5) band', function()
        assert(xi.combat.physical.spikeRatio(true, 0.5) == 0)
        assert(xi.combat.physical.spikeRatio(true, 1.5) == 0)
        assert(xi.combat.physical.spikeRatio(true, 0.4) == 0)
        assert(xi.combat.physical.spikeRatio(true, 1.6) == 0)
        assert(xi.combat.physical.spikeRatio(true, 0) == 0)
        assert(xi.combat.physical.spikeRatio(true, 2) == 0)
    end)

    it('peaks at wRatio 1 and clamps to one-third', function()
        -- (0.5 - 0) * 1.2 = 0.6, clamp to 1/3
        local peak = xi.combat.physical.spikeRatio(true, 1.0)
        assert(math.abs(peak - (1 / 3)) < 1e-12)

        -- Near boundary: wRatio 0.6 → (0.5 - 0.4) * 1.2 = 0.12
        local near = xi.combat.physical.spikeRatio(true, 0.6)
        assert(math.abs(near - 0.12) < 1e-12)

        -- Symmetric about 1: 0.75 and 1.25 → (0.5 - 0.25) * 1.2 = 0.3
        assert(math.abs(xi.combat.physical.spikeRatio(true, 0.75) - 0.3) < 1e-12)
        assert(math.abs(xi.combat.physical.spikeRatio(true, 1.25) - 0.3) < 1e-12)
    end)

    it('uses one-third not 0.33 as the clamp high', function()
        -- Peak would be 0.6 without clamp; ensure we get exact 1/3
        local peak = xi.combat.physical.spikeRatio(true, 1.0)
        assert(peak == 1 / 3)
        assert(peak ~= 0.33)
    end)
end)

describe('pDIF spike ratio non-PC arm', function()
    it('uses the three-piece monster curve', function()
        -- Low band (0, 0.75): formula goes negative below w=0.5 → clamp 0
        assert(xi.combat.physical.spikeRatio(false, 0.4) == 0)

        -- Low band positive sample: w=0.6 → -5/9 + (10/9)*0.6
        local low = xi.combat.physical.spikeRatio(false, 0.6)
        assert(math.abs(low - (-5 / 9 + (10 / 9) * 0.6)) < 1e-12)

        -- Mid band through 1.3 is flat 0.3 (elseif chain: 0.75 falls here)
        assert(xi.combat.physical.spikeRatio(false, 0.75) == 0.3)
        assert(xi.combat.physical.spikeRatio(false, 1.0) == 0.3)
        assert(xi.combat.physical.spikeRatio(false, 1.3) == 0.3)

        -- High band: 5/3 - (270/256)*w, clamped to [0, 0.3]
        local high = xi.combat.physical.spikeRatio(false, 1.5)
        local raw = 5 / 3 - (270 / 256) * 1.5
        raw = utils.clamp(raw, 0, 0.3)
        assert(math.abs(high - raw) < 1e-12)
    end)

    it('falls into the mid flat band at and below wRatio 0', function()
        -- first branch needs wRatio > 0; 0 and negative hit elseif wRatio <= 1.3 → 0.3
        assert(xi.combat.physical.spikeRatio(false, 0) == 0.3)
        assert(xi.combat.physical.spikeRatio(false, -1) == 0.3)
    end)

    it('clamps high arm to [0, 0.3]', function()
        -- large wRatio drives sRatio negative → 0
        assert(xi.combat.physical.spikeRatio(false, 3) == 0)
    end)
end)
