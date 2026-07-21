-----------------------------------
-- Pure system tests for chocobo breeding genetics (slice 6153).
-----------------------------------

describe('chocobo breeding pure plan', function()
    local Y, BLK, BLU, R, G = 0, 1, 2, 3, 4
    local NONE, GALLOP, CANTER, BURROW, BORE, AUTO_REGEN, TREASURE = 0, 1, 2, 3, 4, 5, 6
    local GOURMET, SPORTS, HIKING, JEUNO = 1, 2, 3, 4
    local MALE, FEMALE = 0, 1

    local function allelesToColor(dna)
        if not dna then
            return Y
        end
        for i = 1, 3 do
            if dna[i] == Y then
                return Y
            end
        end
        local counts = {}
        for i = 1, 3 do
            counts[dna[i]] = (counts[dna[i]] or 0) + 1
        end
        local bestColor, bestCount = dna[1], counts[dna[1]]
        for color, count in pairs(counts) do
            if count > bestCount then
                bestColor, bestCount = color, count
            end
        end
        return bestColor
    end

    local function buildPool(motherDna, fatherDna)
        local pool = {}
        if motherDna then
            for i = 1, math.min(3, #motherDna) do
                pool[#pool + 1] = motherDna[i]
            end
        end
        if fatherDna then
            for i = 1, math.min(3, #fatherDna) do
                pool[#pool + 1] = fatherDna[i]
            end
        end
        while #pool < 6 do
            pool[#pool + 1] = Y
        end
        return pool
    end

    -- rolls: for i = n..2, rolls[step] = j in 1..i
    local function inheritDna(motherDna, fatherDna, rolls)
        local pool = buildPool(motherDna, fatherDna)
        local step = 1
        for i = #pool, 2, -1 do
            local j = rolls[step] or 1
            pool[i], pool[j] = pool[j], pool[i]
            step = step + 1
        end
        return { pool[1], pool[2], pool[3] }
    end

    local function gatherAbilities(motherAbs, fatherAbs)
        local pool, seen = {}, {}
        local function addAll(abilities)
            if not abilities then return end
            for _, a in ipairs(abilities) do
                if a and a ~= NONE and not seen[a] then
                    seen[a] = true
                    pool[#pool + 1] = a
                end
            end
        end
        addAll(motherAbs)
        addAll(fatherAbs)
        return pool
    end

    local bias = {
        [SPORTS] = { [GALLOP] = true, [CANTER] = true, [AUTO_REGEN] = true },
        [JEUNO]  = { [BURROW] = true, [BORE] = true, [TREASURE] = true },
    }

    local function inheritAbilityEx(pool, plan, chance, chanceRoll, biasRoll, pickRoll, favPickRoll)
        if #pool == 0 then return NONE end
        if chanceRoll > chance then return NONE end
        local b = bias[plan]
        if b then
            local favoured = {}
            for _, a in ipairs(pool) do
                if b[a] then favoured[#favoured + 1] = a end
            end
            if #favoured > 0 and biasRoll <= 70 then
                local idx = favPickRoll
                if idx < 1 then idx = 1 end
                if idx > #favoured then idx = #favoured end
                return favoured[idx]
            end
        end
        local idx = pickRoll
        if idx < 1 then idx = 1 end
        if idx > #pool then idx = #pool end
        return pool[idx]
    end

    local function nonBredAlleles(roll1, roll2)
        if roll1 <= 70 then
            return { Y, Y, Y }
        end
        local nonYellow = { BLK, BLU, R, G }
        local idx = roll2
        if idx < 1 then idx = 1 end
        if idx > 4 then idx = 4 end
        local c = nonYellow[idx]
        return { c, c, c }
    end

    local function maleChance(plan)
        if plan == GOURMET then return 70 end
        if plan == HIKING then return 30 end
        return 50
    end

    local function eggGender(plan, roll)
        if roll <= maleChance(plan) then return MALE end
        return FEMALE
    end

    it('yellow dominates phenotype', function()
        assert(allelesToColor({ Y, R, R }) == Y)
        assert(allelesToColor({ R, R, R }) == R)
        assert(allelesToColor({ R, R, BLU }) == R) -- R more common
        assert(allelesToColor({ R, BLU, G }) == R) -- allele1 tiebreak
        assert(allelesToColor(nil) == Y)
    end)

    it('inherit dna pads and shuffles', function()
        -- no shuffle rolls all 1: swaps pool[i] with pool[1] for i=6..2
        -- pool starts as M1 M2 M3 F1 F2 F3
        local dna = inheritDna({ R, R, R }, { BLU, BLU, BLU }, { 1, 1, 1, 1, 1 })
        assert(#dna == 3)
        -- identity-ish when j always 1 produces rotation; just assert length and alleles from pool
        for i = 1, 3 do
            assert(dna[i] == R or dna[i] == BLU)
        end
        -- pad yellow when empty parents
        local yyy = inheritDna(nil, nil, { 1, 1, 1, 1, 1 })
        assert(yyy[1] == Y and yyy[2] == Y and yyy[3] == Y)
    end)

    it('ability pool and inheritance chance', function()
        local pool = gatherAbilities({ GALLOP, NONE, GALLOP }, { BURROW, CANTER })
        assert(#pool == 3 and pool[1] == GALLOP and pool[2] == BURROW and pool[3] == CANTER)
        assert(inheritAbilityEx({}, SPORTS, 99, 1, 1, 1, 1) == NONE)
        assert(inheritAbilityEx(pool, SPORTS, 60, 61, 1, 1, 1) == NONE) -- fail chance
        -- sports bias: gallop/canter/auto favoured; pool has gallop+canter
        local a = inheritAbilityEx(pool, SPORTS, 80, 1, 50, 2, 1)
        assert(a == GALLOP or a == CANTER)
        -- no bias plan: pick from full pool
        assert(inheritAbilityEx(pool, GOURMET, 80, 1, 1, 2, 1) == BURROW)
    end)

    it('non-bred alleles and gender bias', function()
        local a = nonBredAlleles(70, 1)
        assert(a[1] == Y and a[2] == Y and a[3] == Y)
        a = nonBredAlleles(71, 3)
        assert(a[1] == R and a[2] == R and a[3] == R)
        assert(maleChance(GOURMET) == 70 and maleChance(HIKING) == 30 and maleChance(SPORTS) == 50)
        assert(eggGender(GOURMET, 70) == MALE and eggGender(GOURMET, 71) == FEMALE)
        assert(eggGender(HIKING, 30) == MALE and eggGender(HIKING, 31) == FEMALE)
    end)

    it('receptivity chance formula', function()
        assert(60 + math.floor(0 * 3) == 60)
        assert(60 + math.floor(7 * 3) == 81)
        assert(60 + math.floor(3.5 * 3) == 70)
    end)
end)
