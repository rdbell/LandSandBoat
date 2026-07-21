-----------------------------------
-- Pure system tests for blue physical hit assembly (slice 6129).
-- Source: scripts/globals/bluemagic.lua usePhysicalSpell ~282–372
-----------------------------------

describe('blue physical hit assembly pure plan', function()
    local MSG_MAGIC_FAIL = 114
    local ATTACK_RANGED = 3

    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function critChance(hasSpellCrit, spellCritPercent, nativeCrit)
        if not hasSpellCrit then
            return 0
        end
        return clamp(spellCritPercent / 100 + nativeCrit, 0.05, 0.95)
    end

    local function offcratioMod(base, physicalPotencyMerit)
        return base * (physicalPotencyMerit + 100) / 100
    end

    local function sneakApplicable(isAoE, attackType, hasSneak, behindOrHide)
        if isAoE or attackType == ATTACK_RANGED then
            return false
        end
        return hasSneak and behindOrHide
    end

    local function hitDamage(finalD, multiplier, correlation, hitIndex, pdif)
        if hitIndex == 0 then
            return finalD * (multiplier + correlation) * pdif
        end
        return finalD * (1 + correlation) * pdif
    end

    local function accumulate(p)
        local finaldmg = 0
        local hitslanded = 0
        local anyCrit = false
        local tpHits = 0
        local sneak = p.sneakApplicable == true
        for i, h in ipairs(p.hits or {}) do
            local landed = sneak or h.chance <= (p.hitrate or 0)
            if landed then
                local pdif = h.pdif or 0
                local isCrit = sneak or (h.critRoll or 1) < (p.critchance or 0)
                if isCrit then
                    pdif = pdif + 1
                end
                anyCrit = anyCrit or isCrit
                finaldmg = finaldmg + hitDamage(p.finalD or 0, p.multiplier or 1, p.correlation or 0, i - 1, pdif)
                hitslanded = hitslanded + 1
                sneak = false
                if finaldmg > 0 then
                    tpHits = tpHits + 1
                end
            end
        end
        finaldmg = math.floor(finaldmg * (p.damageAdj or 1))
        local setFail = hitslanded == 0
        return {
            finaldmg = finaldmg,
            hitslanded = hitslanded,
            anyCrit = anyCrit,
            tphitslanded = tpHits,
            setMsgFail = setFail,
            msg = setFail and MSG_MAGIC_FAIL or nil,
        }
    end

    it('crit chance zero when spell omits critchance', function()
        assert(critChance(false, 50, 0.1) == 0)
    end)

    it('crit chance clamps spell base + native', function()
        assert(critChance(true, 0, 0) == 0.05)
        assert(critChance(true, 100, 0) == 0.95)
        -- 20% + 0.1 native = 0.30
        assert(math.abs(critChance(true, 20, 0.1) - 0.30) < 1e-9)
    end)

    it('offcratio mod scales by physical potency merit', function()
        -- base 100 * (20+100)/100 = 120
        assert(offcratioMod(100, 20) == 120)
        assert(offcratioMod(50, 0) == 50)
    end)

    it('sneak not applicable on AoE or ranged', function()
        assert(sneakApplicable(true, 1, true, true) == false)
        assert(sneakApplicable(false, ATTACK_RANGED, true, true) == false)
        assert(sneakApplicable(false, 1, true, true) == true)
        assert(sneakApplicable(false, 1, true, false) == false)
    end)

    it('first hit uses full multiplier; later hits use 1+corr', function()
        -- finalD 10, mult 2, corr 0.25, pdif 1
        assert(hitDamage(10, 2, 0.25, 0, 1) == 22.5)
        assert(hitDamage(10, 2, 0.25, 1, 1) == 12.5)
    end)

    it('accumulate two hits with damage adj', function()
        local r = accumulate({
            finalD = 10, multiplier = 2, correlation = 0, hitrate = 1, critchance = 0,
            damageAdj = 1.5,
            hits = {
                { chance = 0, pdif = 1, critRoll = 1 },
                { chance = 0, pdif = 1, critRoll = 1 },
            },
        })
        -- hit0: 10*2*1=20; hit1: 10*1*1=10; sum 30 * 1.5 = 45
        assert(r.finaldmg == 45 and r.hitslanded == 2 and r.tphitslanded == 2)
        assert(r.setMsgFail ~= true)
    end)

    it('miss all sets MAGIC_FAIL', function()
        local r = accumulate({
            finalD = 10, hitrate = 0.1, critchance = 0,
            hits = { { chance = 0.5, pdif = 1, critRoll = 1 } },
        })
        assert(r.hitslanded == 0 and r.setMsgFail == true and r.msg == MSG_MAGIC_FAIL)
    end)

    it('sneak forces first hit land and crit then clears', function()
        local r = accumulate({
            finalD = 10, multiplier = 1, correlation = 0, hitrate = 0, critchance = 0,
            sneakApplicable = true, damageAdj = 1,
            hits = {
                { chance = 1, pdif = 1, critRoll = 1 }, -- would miss without sneak
                { chance = 1, pdif = 1, critRoll = 1 }, -- miss (sneak cleared)
            },
        })
        -- only first lands: 10*1*(1+1 crit)=20
        assert(r.hitslanded == 1 and r.finaldmg == 20 and r.anyCrit == true)
    end)

    it('crit adds 1 to pdif', function()
        local r = accumulate({
            finalD = 10, multiplier = 1, correlation = 0, hitrate = 1, critchance = 1,
            damageAdj = 1,
            hits = { { chance = 0, pdif = 1.5, critRoll = 0 } },
        })
        -- pdif 2.5; dmg 25
        assert(r.finaldmg == 25 and r.anyCrit == true)
    end)
end)
