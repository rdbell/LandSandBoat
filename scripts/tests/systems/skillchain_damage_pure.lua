-----------------------------------
-- Pure system tests for skillchain damage dual-wire helpers (slice 6699).
-- Calls production xi.combat.skillchain pure exports.
-- Goldens match internal/skillchaindmg (0870 / 1570).
-----------------------------------

require('scripts/globals/combat/skillchain')

local sc = xi.combat.skillchain
local el = xi.element
local sct = xi.skillchainType

describe('skillchain tables and clamps', function()
    it('pins chain multipliers and res-rank ladder corners', function()
        assert(sc.chainMultipliers[1][1] == 0.50)
        assert(sc.chainMultipliers[1][6] == 1.00)
        assert(sc.chainMultipliers[4][6] == 3.00)
        assert(sc.chainMultiplierFromParams(1, 1) == 0.50)
        assert(sc.chainMultiplierFromParams(4, 6) == 3.00)
        assert(sc.chainMultiplierFromParams(0, 1) == 0)
        assert(sc.chainMultiplierFromParams(5, 1) == 0)

        assert(sc.resistanceRankMultiplier[-3] == 1.50)
        assert(sc.resistanceRankMultiplier[0] == 1.00)
        assert(sc.resistanceRankMultiplier[11] == 0.05)
        assert(sc.resRankMultiplierFromParams(0) == 1.00)
        assert(sc.resRankMultiplierFromParams(-99) == 1.50)
        assert(sc.resRankMultiplierFromParams(99) == 0.05)
        assert(sc.clampResRank(-5) == -3)
        assert(sc.clampResRank(20) == 11)
    end)
end)

describe('hasElement and elementsFor', function()
    it('uses SC type column (not type+1) matching C++/Go', function()
        -- Liquefaction carries Fire
        assert(sc.hasElement(sct.LIQUEFACTION, el.FIRE))
        assert(not sc.hasElement(sct.LIQUEFACTION, el.ICE))
        -- Compression carries Dark
        assert(sc.hasElement(sct.COMPRESSION, el.DARK))
        -- Transfixion carries Light
        assert(sc.hasElement(sct.TRANSFIXION, el.LIGHT))
        -- Gravitation: Earth + Dark
        assert(sc.hasElement(sct.GRAVITATION, el.EARTH))
        assert(sc.hasElement(sct.GRAVITATION, el.DARK))
        assert(not sc.hasElement(sct.NONE, el.FIRE))
        assert(not sc.hasElement(sct.DARKNESS_II, 0))

        local elems = sc.elementsFor(sct.LIQUEFACTION)
        assert(#elems == 1 and elems[1] == el.FIRE)

        elems = sc.elementsFor(sct.GRAVITATION)
        assert(#elems == 2)
        assert(elems[1] == el.EARTH and elems[2] == el.DARK)
    end)
end)

describe('selectElementFromParams', function()
    it('returns sole element or lowest res-rank with reverse ties', function()
        assert(sc.selectElementFromParams({ skillchainType = sct.LIQUEFACTION }) == el.FIRE)

        -- Gravitation Earth+Dark: lower rank wins; ties keep earlier FIRE..DARK via reverse scan
        local ranks = { [el.EARTH] = 2, [el.DARK] = 5 }
        assert(sc.selectElementFromParams({
            skillchainType = sct.GRAVITATION, resRanks = ranks,
        }) == el.EARTH)

        ranks = { [el.EARTH] = 5, [el.DARK] = 2 }
        assert(sc.selectElementFromParams({
            skillchainType = sct.GRAVITATION, resRanks = ranks,
        }) == el.DARK)

        -- Equal ranks: reverse iteration last-wins → earliest FIRE..DARK among tied = Earth
        ranks = { [el.EARTH] = 1, [el.DARK] = 1 }
        assert(sc.selectElementFromParams({
            skillchainType = sct.GRAVITATION, resRanks = ranks,
        }) == el.EARTH)

        assert(sc.selectElementFromParams({ skillchainType = sct.NONE }) == 0)
    end)
end)

describe('skillchainDamageProductFromParams', function()
    it('early-outs and nullification', function()
        local dmg, consume, applied, elem = sc.skillchainDamageProductFromParams({
            hasEffect = false, skillchainType = sct.LIQUEFACTION, skillchainLevel = 1,
            skillchainCount = 1, element = el.FIRE, baseDamage = 1000,
        })
        assert(dmg == 0 and not consume and not applied)

        dmg, consume, applied, elem = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = 0, skillchainLevel = 1,
            skillchainCount = 1, element = el.FIRE, baseDamage = 1000,
        })
        assert(dmg == 0 and not applied)

        dmg, consume, applied, elem = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.LIQUEFACTION, skillchainLevel = 1,
            skillchainCount = 1, element = el.FIRE, baseDamage = 1000, nullified = true,
        })
        assert(dmg == 0 and not consume and not applied and elem == el.FIRE)
    end)

    it('applies level mult and full floor chain golden', function()
        -- L1C1: floor(1000*0.5)=500
        local dmg = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.LIQUEFACTION, skillchainLevel = 1,
            skillchainCount = 1, element = el.FIRE, baseDamage = 1000,
            bonusMult = 1, damageMult = 1, dayWeatherMult = 1, staffMult = 1,
            affinityMult = 1, magicTakenMult = 1, inninMult = 1, sengikoriMult = 1,
            absorbMult = 1, resRank = 0, magicDamage = 0,
        })
        assert(dmg == 500)

        -- abs base
        dmg = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.LIGHT, skillchainLevel = 3,
            skillchainCount = 1, element = el.FIRE, baseDamage = -800,
            bonusMult = 1, damageMult = 1, dayWeatherMult = 1, staffMult = 1,
            affinityMult = 1, magicTakenMult = 1, inninMult = 1, sengikoriMult = 1,
            absorbMult = 1, resRank = 0, magicDamage = 0,
        })
        assert(dmg == 800)

        -- bonus + magic damage: floor(1000*0.5)=500; floor(500*1.2)+50=650
        dmg = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.LIQUEFACTION, skillchainLevel = 1,
            skillchainCount = 1, element = el.FIRE, baseDamage = 1000,
            bonusMult = 1.2, magicDamage = 50,
            damageMult = 1, dayWeatherMult = 1, staffMult = 1, affinityMult = 1,
            magicTakenMult = 1, inninMult = 1, sengikoriMult = 1, absorbMult = 1, resRank = 0,
        })
        assert(dmg == 650)

        -- Full chain golden (matches Go TestProductFullChainGolden)
        dmg = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.FUSION, skillchainLevel = 2,
            skillchainCount = 3, element = el.FIRE, baseDamage = 1000,
            bonusMult = 1.10, damageMult = 1.05, dayWeatherMult = 1.10, staffMult = 1.05,
            affinityMult = 1.02, magicTakenMult = 0.90, inninMult = 1.05, sengikoriMult = 1.10,
            absorbMult = 1, resRank = 0, magicDamage = 25,
        })
        assert(dmg == 1444)

        -- Absorb negative
        dmg = sc.skillchainDamageProductFromParams({
            hasEffect = true, skillchainType = sct.COMPRESSION, skillchainLevel = 1,
            skillchainCount = 2, element = el.DARK, baseDamage = 500,
            bonusMult = 1, damageMult = 1, dayWeatherMult = 1, staffMult = 1,
            affinityMult = 1, magicTakenMult = 1, inninMult = 1, sengikoriMult = 1,
            absorbMult = -1, resRank = 0, magicDamage = 0,
        })
        assert(dmg == -300)
    end)
end)
