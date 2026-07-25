-----------------------------------
-- Treasure Hunter drop-rate helpers.
-- Pure table + getDropRate dual-wired to OmegaXI internal/treasurehunter (slice 6694 / 0834).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.treasureHunter = xi.combat.treasureHunter or {}
-----------------------------------

xi.combat.treasureHunter.maxTier = 14
xi.combat.treasureHunter.maxDropRate = 10000
xi.combat.treasureHunter.bracketCount = 7

-- https://forum.square-enix.com/ffxi/threads/56550
xi.combat.treasureHunter.treasureHunterTable =
{
-- TH lvl    VC    C     UC    R     VR    SR   UR
    [ 0] = { 2400, 1500, 1000,  500,  100,  50,  10 },
    [ 1] = { 4800, 3000, 1200,  600,  150,  75,  20 },
    [ 2] = { 5600, 4000, 1500,  700,  200, 100,  30 },
    [ 3] = { 6000, 4250, 1650,  750,  225, 120,  35 },
    [ 4] = { 6400, 4500, 1800,  800,  250, 140,  40 },
    [ 5] = { 6666, 4750, 1900,  850,  300, 160,  45 },
    [ 6] = { 6800, 5000, 2000,  900,  350, 180,  50 },
    [ 7] = { 6900, 5250, 2100,  950,  400, 200,  60 },
    [ 8] = { 7050, 5500, 2250, 1050,  475, 230,  70 },
    [ 9] = { 7200, 5750, 2400, 1150,  550, 260,  80 },
    [10] = { 7350, 6000, 2650, 1250,  650, 300,  90 },
    [11] = { 7400, 6250, 2800, 1350,  750, 350, 100 },
    [12] = { 7600, 6500, 2950, 1550,  825, 400, 115 },
    [13] = { 7800, 6750, 3100, 1750,  900, 450, 130 },
    [14] = { 8000, 7000, 3250, 2000, 1000, 500, 150 },
}

xi.combat.treasureHunter.dropBracketTable =
{
    [1] = { 2400 },
    [2] = { 1500 },
    [3] = { 1000 },
    [4] = {  500 },
    [5] = {  100 },
    [6] = {   50 },
    [7] = {    0 }, -- Set to 0, for weird cases in DB.
}

-- Pure clamp helpers (parity with Go ClampTier / ClampDropRate).
xi.combat.treasureHunter.clampTier = function(thLevel)
    return utils.clamp(utils.defaultIfNil(thLevel, 0), 0, xi.combat.treasureHunter.maxTier)
end

xi.combat.treasureHunter.clampDropRate = function(dropRate)
    return utils.clamp(utils.defaultIfNil(dropRate, 0), 0, xi.combat.treasureHunter.maxDropRate)
end

-- Pure rarity bracket (1..7) for a base drop rate on the 1/10000 scale.
xi.combat.treasureHunter.dropBracket = function(dropRate)
    local thDropRate = xi.combat.treasureHunter.clampDropRate(dropRate)

    for i = 1, #xi.combat.treasureHunter.dropBracketTable do
        if thDropRate >= xi.combat.treasureHunter.dropBracketTable[i][1] then
            return i
        end
    end

    return 0
end

-- Pure getDropRate: TH tier + base drop rate → remapped 1/10000 drop rate.
-- Already entity-free; dual-wired to internal/treasurehunter.GetDropRate.
xi.combat.treasureHunter.getDropRate = function(thLevel, dropRate)
    local thTier = xi.combat.treasureHunter.clampTier(thLevel)
    local thDropRate = xi.combat.treasureHunter.clampDropRate(dropRate)

    -- Early returns: Drop is guaranteed or non-existant.
    if thDropRate == xi.combat.treasureHunter.maxDropRate then
        return xi.combat.treasureHunter.maxDropRate
    elseif thDropRate == 0 then
        return 0
    end

    local thBracket = xi.combat.treasureHunter.dropBracket(thDropRate)

    return xi.combat.treasureHunter.treasureHunterTable[thTier][thBracket]
end
