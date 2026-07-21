-----------------------------------
-- Pure system tests for blue useCuringSpell + applySpellDamage (slice 6126).
-- Source: scripts/globals/bluemagic.lua ~636–679, ~764–793
-----------------------------------

describe('blue useCuringSpell pure plan', function()
    local MSG_MAGIC_RECOVERS_HP = 7
    local MSG_SELF_HEAL_SECONDARY = 263

    local function selectLadder(power, t1, t2, d0, c0, d1, c1, d2, c2)
        if power > t2 then
            return d2, c2
        end
        if power > t1 then
            return d1, c1
        end
        return d0, c0
    end

    local function useCuring(p)
        local final = p.cureFinal or 0 -- getCureFinal inject
        final = final + final * (p.curePotencyRcvd or 0) / 100
        final = final * (p.curePower or 1)
        local missing = (p.maxHP or 0) - (p.curHP or 0)
        if final < 0 then final = 0 end
        if final > missing then final = missing end
        final = math.floor(final) -- LSB multiplies may leave float; clamp returns number
        -- actually clamp is on already scaled final; floor not explicit but Lua numbers
        local msg = MSG_MAGIC_RECOVERS_HP
        if not p.isPrimaryTarget then
            msg = MSG_SELF_HEAL_SECONDARY
        end
        return {
            final = final,
            msg = msg,
            addHP = true,
            wakeUp = true,
            updateEnmityFromCure = true,
        }
    end

    it('ladder selects by power thresholds', function()
        local d, c = selectLadder(50, 100, 200, 1, 10, 2, 20, 3, 30)
        assert(d == 1 and c == 10)
        d, c = selectLadder(150, 100, 200, 1, 10, 2, 20, 3, 30)
        assert(d == 2 and c == 20)
        d, c = selectLadder(250, 100, 200, 1, 10, 2, 20, 3, 30)
        assert(d == 3 and c == 30)
        -- equal to threshold uses lower band (only > triggers)
        d, c = selectLadder(100, 100, 200, 1, 10, 2, 20, 3, 30)
        assert(d == 1 and c == 10)
    end)

    it('rcvd potency and cure power then clamp to missing HP', function()
        -- final 100 + 20% rcvd = 120; * 1.1 cure power = 132; missing 50 → 50
        local r = useCuring({
            cureFinal = 100, curePotencyRcvd = 20, curePower = 1.1,
            maxHP = 200, curHP = 150, isPrimaryTarget = true,
        })
        assert(r.final == 50 and r.msg == MSG_MAGIC_RECOVERS_HP)
    end)

    it('secondary target message', function()
        local r = useCuring({
            cureFinal = 40, curePower = 1, maxHP = 100, curHP = 50, isPrimaryTarget = false,
        })
        assert(r.msg == MSG_SELF_HEAL_SECONDARY and r.final == 40)
    end)

    it('zero missing HP yields zero heal', function()
        local r = useCuring({
            cureFinal = 99, curePower = 1, maxHP = 100, curHP = 100, isPrimaryTarget = true,
        })
        assert(r.final == 0)
    end)
end)

describe('blue applySpellDamage pure plan', function()
    local ATTACK_MAGICAL = 2

    local function apply(p)
        local dmg = math.floor((p.dmg or 0) * (p.bluePower or 1))
        local extraTP = math.floor((p.tpGain or 0) * math.max((p.tpHits or 0) - 1, 0))
        -- note: LSB does not floor extraTP product; keep as float multiply then use as-is
        extraTP = (p.tpGain or 0) * math.max((p.tpHits or 0) - 1, 0)

        if p.attackType == ATTACK_MAGICAL then
            dmg = math.floor(dmg * (p.absorb or 1) * (p.nullify or 1))
            if dmg < 0 then
                return {
                    returnDamage = dmg, takeSpellDamage = true, addTP = true,
                    extraTP = extraTP, absorbPath = true,
                }
            end
            dmg = p.oneForAll and p.oneForAll(dmg) or dmg
        end

        if p.phalanx then dmg = p.phalanx(dmg) end
        if p.stoneskin then dmg = p.stoneskin(dmg) end
        if p.damageCap then dmg = p.damageCap(dmg) end

        return {
            returnDamage = dmg,
            takeSpellDamage = true,
            addTP = true,
            extraTP = extraTP,
            updateEnmity = not p.targetIsPC,
            enmityFromTrickAttack = p.trickAttackTarget == true,
            handleAfflatus = true,
        }
    end

    it('blue power multiplies first', function()
        local r = apply({ dmg = 100, bluePower = 1.5, attackType = 1 })
        assert(r.returnDamage == 150)
    end)

    it('magical absorb negative early path', function()
        local r = apply({
            dmg = 100, bluePower = 1, attackType = ATTACK_MAGICAL,
            absorb = -1, nullify = 1, tpHits = 1, tpGain = 50,
        })
        assert(r.absorbPath == true and r.returnDamage == -100)
        assert(r.extraTP == 0) -- max(1-1,0)=0
    end)

    it('extra TP from multi-hits', function()
        local r = apply({
            dmg = 10, bluePower = 1, attackType = 1, tpHits = 3, tpGain = 40,
        })
        assert(r.extraTP == 80)
    end)

    it('pc skips enmity', function()
        local r = apply({ dmg = 10, bluePower = 1, attackType = 1, targetIsPC = true })
        assert(r.updateEnmity ~= true)
    end)

    it('trick attack enmity source flag', function()
        local r = apply({
            dmg = 10, bluePower = 1, attackType = 1,
            targetIsPC = false, trickAttackTarget = true,
        })
        assert(r.enmityFromTrickAttack == true and r.updateEnmity == true)
    end)
end)
