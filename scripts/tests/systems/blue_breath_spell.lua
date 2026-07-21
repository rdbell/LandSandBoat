-----------------------------------
-- Pure system tests for blue breath base + product + Use plan (slice 6123).
-- Source: scripts/globals/bluemagic.lua useBreathSpell ~540–633
-----------------------------------

describe('blue breath spell pure plan', function()
    local MSG_MAGIC_RECOVERS_HP = 7 -- xi.msg.basic.MAGIC_RECOVERS_HP

    local function breathBase(p)
        if p.isConal and not p.targetInFront then
            return { missedConal = true, base = 0 }
        end
        local dmg = (p.casterHP or 0) / (p.hpMod or 1)
        if (p.lvlMod or 0) > 0 then
            dmg = dmg + (p.mainLevel or 0) / p.lvlMod
        end
        return { missedConal = false, base = dmg }
    end

    local function breathProduct(base, m)
        local dmg = base
        local keys = {
            'correlation', 'breathSDT', 'absorb', 'nullify', 'tmda',
            'staff', 'affinity', 'resist', 'addResist', 'elemSDT',
            'dayWeather', 'mab', 'skillType', 'futae', 'ninjutsu',
            'scarlet', 'aoe', 'nukeWall',
        }
        for _, k in ipairs(keys) do
            dmg = math.floor(dmg * (m[k] or 1))
        end
        return dmg
    end

    local function use(p)
        if p.missedConal then
            return { returnDamage = 0, applyDamage = false }
        end
        local dmg = p.productDamage or 0
        if dmg < 0 then
            return {
                returnDamage = p.recoverHP or -dmg, -- host addHP(-dmg)
                setMsg = true,
                msg = MSG_MAGIC_RECOVERS_HP,
                recoverHP = true,
                applyDamage = false,
            }
        end
        -- host severe/phalanx/1forall/stoneskin/cap inject as finalDamage
        local final = p.finalDamage
        if final == nil then
            final = dmg
        end
        return {
            returnDamage = final,
            applyDamage = final > 0 or final == 0, -- takeSpellDamage always
            takeSpellDamage = true,
            handleSevere = dmg > 0,
        }
    end

    it('conal miss returns 0', function()
        local b = breathBase({ isConal = true, targetInFront = false, casterHP = 1000, hpMod = 2 })
        assert(b.missedConal == true)
        local r = use({ missedConal = true })
        assert(r.returnDamage == 0 and r.applyDamage == false)
    end)

    it('base is HP/hpMod plus level/lvlMod', function()
        -- 1000/4 + 75/5 = 250 + 15 = 265
        local b = breathBase({ casterHP = 1000, hpMod = 4, mainLevel = 75, lvlMod = 5 })
        assert(b.base == 265 and b.missedConal ~= true)
    end)

    it('lvlMod zero skips level term', function()
        local b = breathBase({ casterHP = 800, hpMod = 2, mainLevel = 99, lvlMod = 0 })
        assert(b.base == 400)
    end)

    it('non-conal ignores front check', function()
        local b = breathBase({ isConal = false, targetInFront = false, casterHP = 100, hpMod = 1 })
        assert(b.missedConal ~= true and b.base == 100)
    end)

    it('product floor chain', function()
        -- base 100 * 1.25 corr = 125; * 0.5 resist = 62
        local d = breathProduct(100, { correlation = 1.25, resist = 0.5 })
        assert(d == 62)
    end)

    it('absorb negative triggers recover path', function()
        local r = use({ productDamage = -50, recoverHP = 50 })
        assert(r.recoverHP == true and r.msg == MSG_MAGIC_RECOVERS_HP)
        assert(r.returnDamage == 50 and r.applyDamage == false)
    end)

    it('positive damage takeSpellDamage', function()
        local r = use({ productDamage = 200, finalDamage = 180 })
        assert(r.takeSpellDamage == true and r.returnDamage == 180)
        assert(r.handleSevere == true)
    end)

    it('zero product still takeSpellDamage', function()
        local r = use({ productDamage = 0, finalDamage = 0 })
        assert(r.takeSpellDamage == true and r.returnDamage == 0)
        assert(r.handleSevere ~= true)
    end)
end)
