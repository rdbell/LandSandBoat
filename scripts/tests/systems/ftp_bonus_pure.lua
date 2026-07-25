-----------------------------------
-- Pure system tests for WS fTP gear-bonus dual-wire helpers (slice 6690).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/ftpbonus (0851).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical
local sc = xi.skillchainType
local el = xi.element

describe('ftp bonus constants and wsElementalProperties', function()
    it('pins divisor and Lv1 single-element rows', function()
        assert(phys.ftpDivisor == 256)
        assert(phys.wsElementalProperties[sc.NONE][1] == 0)
        assert(phys.wsElementalProperties[sc.LIQUEFACTION][1] == 1) -- Fire
        assert(phys.wsElementalProperties[sc.INDURATION][2] == 1)   -- Ice
        assert(phys.wsElementalProperties[sc.DETONATION][3] == 1)   -- Wind
        assert(phys.wsElementalProperties[sc.SCISSION][4] == 1)     -- Earth
        assert(phys.wsElementalProperties[sc.IMPACTION][5] == 1)    -- Thunder
        assert(phys.wsElementalProperties[sc.REVERBERATION][6] == 1)-- Water
        assert(phys.wsElementalProperties[sc.TRANSFIXION][7] == 1)  -- Light
        assert(phys.wsElementalProperties[sc.COMPRESSION][8] == 1)  -- Dark
    end)

    it('pins Lv2 pairs and Lv3/Lv4 quads', function()
        -- Fusion = Fire + Light
        local fusion = phys.wsElementalProperties[sc.FUSION]
        assert(fusion[1] == 1 and fusion[7] == 1 and fusion[2] == 0)
        -- Distortion = Ice + Water
        local dist = phys.wsElementalProperties[sc.DISTORTION]
        assert(dist[2] == 1 and dist[6] == 1)
        -- Fragmentation = Wind + Thunder
        local frag = phys.wsElementalProperties[sc.FRAGMENTATION]
        assert(frag[3] == 1 and frag[5] == 1)
        -- Gravitation = Earth + Dark
        local grav = phys.wsElementalProperties[sc.GRAVITATION]
        assert(grav[4] == 1 and grav[8] == 1)
        -- Light / Light_II
        local light = phys.wsElementalProperties[sc.LIGHT]
        assert(light[1] == 1 and light[3] == 1 and light[5] == 1 and light[7] == 1)
        assert(light[2] == 0 and light[4] == 0)
        local light2 = phys.wsElementalProperties[sc.LIGHT_II]
        for i = 1, 8 do
            assert(light2[i] == light[i])
        end
        -- Darkness / Darkness_II
        local dark = phys.wsElementalProperties[sc.DARKNESS]
        assert(dark[2] == 1 and dark[4] == 1 and dark[6] == 1 and dark[8] == 1)
        local dark2 = phys.wsElementalProperties[sc.DARKNESS_II]
        for i = 1, 8 do
            assert(dark2[i] == dark[i])
        end
    end)
end)

describe('hasWSElement and propsCarryElement', function()
    it('reports single-element membership', function()
        assert(phys.hasWSElement(sc.LIQUEFACTION, el.FIRE))
        assert(not phys.hasWSElement(sc.LIQUEFACTION, el.ICE))
        assert(not phys.hasWSElement(sc.NONE, el.FIRE))
        assert(not phys.hasWSElement(sc.LIQUEFACTION, 0))
        assert(not phys.hasWSElement(sc.LIQUEFACTION, 9))
        assert(not phys.hasWSElement(99, el.FIRE))
    end)

    it('ORs three WS props', function()
        assert(phys.propsCarryElement(sc.LIQUEFACTION, sc.NONE, sc.COMPRESSION, el.FIRE))
        assert(phys.propsCarryElement(sc.LIQUEFACTION, sc.NONE, sc.COMPRESSION, el.DARK))
        assert(not phys.propsCarryElement(sc.LIQUEFACTION, sc.NONE, sc.COMPRESSION, el.ICE))
        assert(not phys.propsCarryElement(sc.NONE, sc.NONE, sc.NONE, el.FIRE))
    end)
end)

describe('ftpBonusFromParams', function()
    it('returns 0 for non-PC and nonelemental WS', function()
        assert(phys.ftpBonusFromParams({
            isPC = false, scProp1 = sc.LIQUEFACTION,
            elementFTPMods = { [el.FIRE] = 256 }, anyFTPBonus = 128,
        }) == 0)
        assert(phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.NONE, scProp2 = sc.NONE, scProp3 = sc.NONE,
            anyFTPBonus = 256, elementFTPMods = { [el.FIRE] = 256 },
        }) == 0)
    end)

    it('applies single element mod / 256', function()
        local got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION,
            elementFTPMods = { [el.FIRE] = 26 },
        })
        assert(math.abs(got - 26 / 256) < 1e-12)

        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION,
            elementFTPMods = { [el.ICE] = 100 },
        })
        assert(got == 0)
    end)

    it('applies day FTP bonus only when day element is carried', function()
        local got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION, dayElement = el.FIRE,
            elementFTPMods = { [el.FIRE] = 26 }, dayFTPBonus = 10,
        })
        assert(math.abs(got - (26 + 10) / 256) < 1e-12)

        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION, dayElement = el.ICE,
            elementFTPMods = { [el.FIRE] = 26 }, dayFTPBonus = 10,
        })
        assert(math.abs(got - 26 / 256) < 1e-12)
    end)

    it('applies ANY_FTP_BONUS after elemental gates', function()
        local got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION, anyFTPBonus = 32,
        })
        assert(math.abs(got - 32 / 256) < 1e-12)

        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION,
            elementFTPMods = { [el.FIRE] = 26 }, anyFTPBonus = 32,
        })
        assert(math.abs(got - (26 + 32) / 256) < 1e-12)
    end)

    it('sums multi-element props without double-counting Fire', function()
        local got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.FUSION, dayElement = el.LIGHT,
            elementFTPMods = { [el.FIRE] = 26, [el.LIGHT] = 13, [el.ICE] = 99 },
            dayFTPBonus = 10, anyFTPBonus = 5,
        })
        assert(math.abs(got - (26 + 13 + 10 + 5) / 256) < 1e-12)

        -- Overlapping Liquefaction + Fusion both carry Fire once
        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIQUEFACTION, scProp2 = sc.FUSION,
            elementFTPMods = { [el.FIRE] = 50, [el.LIGHT] = 25 },
        })
        assert(math.abs(got - (50 + 25) / 256) < 1e-12)
    end)

    it('handles Light family day once and Fragmentation assembly', function()
        local got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIGHT, dayElement = el.FIRE, dayFTPBonus = 16,
        })
        assert(math.abs(got - 16 / 256) < 1e-12)

        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.LIGHT, dayElement = el.ICE, dayFTPBonus = 16,
        })
        assert(got == 0)

        got = phys.ftpBonusFromParams({
            isPC = true, scProp1 = sc.FRAGMENTATION, scProp2 = sc.COMPRESSION,
            dayElement = el.THUNDER,
            elementFTPMods = {
                [el.WIND] = 8, [el.THUNDER] = 16, [el.DARK] = 4, [el.ICE] = 99,
            },
            dayFTPBonus = 16, anyFTPBonus = 2,
        })
        assert(math.abs(got - (8 + 16 + 16 + 4 + 2) / 256) < 1e-12)
    end)

    it('pins Lv1 unit-mod ladder to 1/256', function()
        local ladder = {
            { sc.LIQUEFACTION, el.FIRE },
            { sc.INDURATION, el.ICE },
            { sc.DETONATION, el.WIND },
            { sc.SCISSION, el.EARTH },
            { sc.IMPACTION, el.THUNDER },
            { sc.REVERBERATION, el.WATER },
            { sc.TRANSFIXION, el.LIGHT },
            { sc.COMPRESSION, el.DARK },
        }
        for _, tc in ipairs(ladder) do
            local mods = { [tc[2]] = 1 }
            local got = phys.ftpBonusFromParams({
                isPC = true, scProp1 = tc[1], elementFTPMods = mods,
            })
            assert(math.abs(got - 1 / 256) < 1e-12)
        end
    end)
end)
