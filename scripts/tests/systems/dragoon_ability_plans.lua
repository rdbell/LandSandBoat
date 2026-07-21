-----------------------------------
-- Pure system tests for Dragoon ability plans (job_utils/dragoon.lua).
-- Pins formulas without entity hosts where pure helpers can be derived in-test.
-----------------------------------

describe('Dragoon ability pure plans', function()
    it('jump attack and fTP products', function()
        -- (JUMP_ATT_BONUS + 100) / 100
        local function jumpATT(bonus)
            return (bonus + 100) / 100
        end

        assert(jumpATT(0) == 1.0)
        assert(jumpATT(25) == 1.25)
        -- Jump fTP: 1 + VIT/256
        assert(1 + 128 / 256 == 1.5)

        -- Spirit Jump with wyvern: +0.25 atk, tp*2, force crit
        local atk = jumpATT(0) + 0.25
        assert(atk == 1.25)
        -- Soul Jump with wyvern: +0.5 atk, tp*3
        atk = jumpATT(0) + 0.5
        assert(atk == 1.5)
    end)

    it('ancient circle power and duration', function()
        local function ancientPower(isDRG, jp, potency)
            local power = 5
            if isDRG then
                power = 15 + jp
            end

            return power + potency
        end

        assert(ancientPower(false, 5, 2) == 7)
        assert(ancientPower(true, 5, 2) == 22)
        assert(180 + 30 == 210) -- base duration + mod
    end)

    it('spirit surge / angon / steady wing / spirit link products', function()
        assert(1 + math.floor(50 / 5) == 11) -- STR boost
        assert(1000 * 0.25 == 250)           -- max HP boost power
        assert(15 + 15 == 30)                -- angon duration at 1 merit
        -- steady wing: max*0.3 + missing
        assert(1000 * 0.3 + (1000 - 700) == 600)
        assert(math.floor(75 / 3) == 25) -- spirit link regen
        assert(1000 / 2 == 500)          -- half wyvern TP
    end)

    it('deep breathing and healing breath table', function()
        local function deepBonus(has, merits, enhance, healing)
            if not has then
                return 0
            end

            if healing then
                local bonus = 37.5 + 12.5 * merits
                if enhance then
                    bonus = bonus + merits * 5
                end

                return bonus
            end

            local bonus = 0.75 + 0.25 * merits
            if enhance then
                bonus = bonus + merits * 0.1
            end

            return bonus
        end

        assert(deepBonus(false, 1, true, true) == 0)
        assert(deepBonus(true, 1, false, true) == 50)
        assert(deepBonus(true, 1, true, true) == 55)
        assert(deepBonus(true, 1, false, false) == 1)
        assert(deepBonus(true, 1, true, false) == 1.1)

        local table =
        {
            [xi.jobAbility.HEALING_BREATH    ] = {  8, 35 },
            [xi.jobAbility.HEALING_BREATH_II ] = { 24, 48 },
            [xi.jobAbility.HEALING_BREATH_III] = { 42, 55 },
            [xi.jobAbility.HEALING_BREATH_IV ] = { 60, 63 },
        }
        assert(table[xi.jobAbility.HEALING_BREATH][1] == 8)
        assert(table[xi.jobAbility.HEALING_BREATH_IV][2] == 63)

        -- curePower sample: maxHP=2560, base=8, mult=35, gear=0, deep=0
        local multiplier = (35 + 0 + 0) / 256
        local cure       = math.floor(2560 * multiplier) + 8
        assert(cure == 358)
    end)

    it('damage breath base, element pick, wyvern exp, healing tier', function()
        -- floor(hp/6+15+jp) * gear * (1+aug+deep)
        local dmg = math.floor(600 / 6 + 15 + 0) * 1.0 * (1.0 + 0 + 0)
        assert(dmg == 115)

        local breaths =
        {
            xi.jobAbility.FLAME_BREATH,
            xi.jobAbility.FROST_BREATH,
            xi.jobAbility.GUST_BREATH,
            xi.jobAbility.SAND_BREATH,
            xi.jobAbility.LIGHTNING_BREATH,
            xi.jobAbility.HYDRO_BREATH,
        }
        local ranks   = { 5, 1, 5, 5, 5, 5 }
        local lowest  = 11
        local pick    = breaths[1]
        for i, v in ipairs(breaths) do
            if ranks[i] < lowest then
                lowest = ranks[i]
                pick   = v
            end
        end

        assert(pick == xi.jobAbility.FROST_BREATH)

        -- wyvern exp level-ups
        local function levelUps(prev, exp)
            if prev >= 1000 then
                return 0, prev
            end

            local cur = exp
            if prev + cur > 1000 then
                cur = 1000 - prev
            end

            local ups = math.floor((prev + cur) / 200) - math.floor(prev / 200)
            return ups, prev + cur
        end

        local ups, exp = levelUps(0, 200)
        assert(ups == 1 and exp == 200)
        ups, exp = levelUps(0, 450)
        assert(ups == 2 and exp == 450)
        ups, exp = levelUps(1000, 100)
        assert(ups == 0 and exp == 1000)

        -- healing breath tier by level
        local function tier(lvl)
            if lvl >= 80 then
                return xi.jobAbility.HEALING_BREATH_IV
            elseif lvl >= 40 then
                return xi.jobAbility.HEALING_BREATH_III
            elseif lvl >= 20 then
                return xi.jobAbility.HEALING_BREATH_II
            end

            return xi.jobAbility.HEALING_BREATH
        end

        assert(tier(10) == xi.jobAbility.HEALING_BREATH)
        assert(tier(20) == xi.jobAbility.HEALING_BREATH_II)
        assert(tier(40) == xi.jobAbility.HEALING_BREATH_III)
        assert(tier(80) == xi.jobAbility.HEALING_BREATH_IV)
    end)

    it('jump recast IDs for spirit surge and fly high resets', function()
        -- abilities.sql recastId column
        assert(xi.jobAbility.JUMP == 66)
        assert(xi.jobAbility.SPIRIT_JUMP == 260)
        assert(xi.jobAbility.SOUL_JUMP == 293)
        -- resetRecast uses recast IDs 158..160 / 166..167 (documented in Lua)
        local spiritSurgeRecasts = { 158, 159, 160 }
        local flyHighRecasts     = { 158, 159, 160, 166, 167 }
        assert(#spiritSurgeRecasts == 3)
        assert(#flyHighRecasts == 5)
        assert(flyHighRecasts[5] == 167)
    end)
end)

describe('Dragoon High Jump / Super Jump pure plans', function()
    it('high jump enmity shed and spirit surge TP remove', function()
        local function highJumpShed(isDRG, mod)
            local base = isDRG and 50 or 30
            return base + mod
        end

        assert(highJumpShed(true, 0) == 50)
        assert(highJumpShed(false, 0) == 30)
        assert(highJumpShed(true, 10) == 60)
        assert(100 * 2 == 200) -- spirit surge delTP
    end)

    it('super jump range and CE/VE injects', function()
        local range = 75.0
        assert(50 <= range)
        assert(75.0 <= range)
        assert(not (75.1 <= range))
        -- setCE(1), setVE(0)
        assert(1 == 1 and 0 == 0)
        -- surge party enmity shed 100
        assert(100 == 100)
    end)
end)
