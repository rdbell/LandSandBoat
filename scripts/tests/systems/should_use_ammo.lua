-----------------------------------
-- Pure system tests for shouldUseAmmo (slice 6102).
-- Source: scripts/globals/combat/ranged_utilities.lua ~117–134
-----------------------------------

describe('shouldUseAmmo pure injects', function()
    -- Lua always sums RECYCLE mod + RECYCLE merit + AMMO_CONSUMPTION JP
    -- (no Recycle-trait gate, unlike the C++ OnRangedAttack path).
    local function recycleChance(recycleMod, recycleMerit, ammoConsumptionJP)
        return recycleMod + recycleMerit + ammoConsumptionJP
    end

    -- Unlimited Shot forces chance to 100 and requests status delete.
    local function effectiveChance(baseChance, hasUnlimitedShot)
        if hasUnlimitedShot then
            return 100
        end
        return baseChance
    end

    -- math.random(1, 100) <= recycleChance → do not consume ammo
    local function shouldUseAmmo(p)
        if not p.isPC then
            return { useAmmo = false, deleteUnlimitedShot = false }
        end

        local deleteUS = p.hasUnlimitedShot == true
        local chance = effectiveChance(
            recycleChance(p.recycleMod or 0, p.recycleMerit or 0, p.ammoConsumptionJP or 0),
            deleteUS
        )
        local roll = p.roll or 1

        if roll <= chance then
            return { useAmmo = false, deleteUnlimitedShot = deleteUS }
        end

        return { useAmmo = true, deleteUnlimitedShot = deleteUS }
    end

    it('non-PC never uses ammo', function()
        local r = shouldUseAmmo({ isPC = false, recycleMod = 0, roll = 100 })
        assert(r.useAmmo == false and r.deleteUnlimitedShot == false)
    end)

    it('PC with zero chance always uses ammo on any roll', function()
        local r = shouldUseAmmo({ isPC = true, recycleMod = 0, recycleMerit = 0, ammoConsumptionJP = 0, roll = 1 })
        assert(r.useAmmo == true)
        r = shouldUseAmmo({ isPC = true, recycleMod = 0, roll = 100 })
        assert(r.useAmmo == true)
    end)

    it('roll at chance boundary recycles (no ammo use)', function()
        local r = shouldUseAmmo({ isPC = true, recycleMod = 25, roll = 25 })
        assert(r.useAmmo == false)
    end)

    it('roll just above chance consumes ammo', function()
        local r = shouldUseAmmo({ isPC = true, recycleMod = 25, roll = 26 })
        assert(r.useAmmo == true)
    end)

    it('sums mod + merit + JP without trait gate', function()
        -- 10 + 5 + 3 = 18
        local r = shouldUseAmmo({
            isPC = true, recycleMod = 10, recycleMerit = 5, ammoConsumptionJP = 3, roll = 18,
        })
        assert(r.useAmmo == false)
        r = shouldUseAmmo({
            isPC = true, recycleMod = 10, recycleMerit = 5, ammoConsumptionJP = 3, roll = 19,
        })
        assert(r.useAmmo == true)
    end)

    it('Unlimited Shot forces 100% recycle and delete flag', function()
        local r = shouldUseAmmo({
            isPC = true, recycleMod = 0, hasUnlimitedShot = true, roll = 100,
        })
        assert(r.useAmmo == false and r.deleteUnlimitedShot == true)

        -- roll 100 still recycles when chance forced to 100
        r = shouldUseAmmo({
            isPC = true, recycleMod = 0, hasUnlimitedShot = true, roll = 1,
        })
        assert(r.useAmmo == false and r.deleteUnlimitedShot == true)
    end)

    it('Unlimited Shot delete flag set even when base chance would consume', function()
        -- Without US: roll 50, chance 0 → use ammo. With US: chance 100 → no use.
        local r = shouldUseAmmo({
            isPC = true, recycleMod = 0, hasUnlimitedShot = true, roll = 50,
        })
        assert(r.useAmmo == false and r.deleteUnlimitedShot == true)
    end)

    it('no Unlimited Shot leaves delete flag false', function()
        local r = shouldUseAmmo({ isPC = true, recycleMod = 50, hasUnlimitedShot = false, roll = 1 })
        assert(r.deleteUnlimitedShot == false)
    end)
end)
