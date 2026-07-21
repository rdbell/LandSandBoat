-----------------------------------
-- Pure system tests for useDamageSpell full plan (slice 6108).
-- Source: scripts/globals/spells/damage_spell.lua ~1084–1210
-----------------------------------

describe('useDamageSpell pure execute plan', function()
    local DAMAGE_TYPE_ELEMENTAL = 5
    local MSG_MAGIC_RECOVERS_HP = 7
    local MSG_MAGIC_RESIST = 85
    local MSG_MAGIC_BURST_DAMAGE = 252

    local function product(p)
        if p.nullified then
            return 0
        end
        local final = p.base or 0
        local mults = p.mults or { 1 }
        for i = 1, #mults do
            final = math.floor(final * mults[i])
        end
        return final
    end

    local function clampDmg(damage)
        if damage < 0 then return 0 end
        if damage > 99999 then return 99999 end
        return damage
    end

    local function mitigation(damage, phalanx, ofaPower, stoneskin, ofaActive)
        if damage <= 0 then return damage end
        damage = damage - (phalanx or 0)
        if damage < 0 then damage = 0 end
        if ofaActive then
            damage = damage - (ofaPower or 0)
            if damage < 0 then damage = 0 end
        end
        if stoneskin and stoneskin > 0 then
            if stoneskin >= damage then
                damage = 0
            else
                damage = damage - stoneskin
            end
        end
        return clampDmg(damage)
    end

    local function applyCap(damage, capActive, capValue)
        if not capActive or damage <= 0 then return damage end
        if damage > capValue then return capValue end
        return damage
    end

    local function execute(p)
        if p.nullified then
            return {
                final = 0, nullified = true,
                spellMsg = MSG_MAGIC_RESIST, setSpellMsg = true,
            }
        end
        local final = product(p)
        if final < 0 then
            local healed = -final
            if p.useAbsorbHealed then
                healed = p.absorbHealed or 0
            end
            return {
                final = healed, absorbHP = true,
                spellMsg = MSG_MAGIC_RECOVERS_HP, setSpellMsg = true,
            }
        end
        final = mitigation(final, p.phalanxMod, p.ofaPower, p.stoneskin, p.ofaActive)
        final = applyCap(final, p.capActive, p.capValue or 0)
        local r = { final = final, takeSpellDamage = final > 0 }
        if final > 0 then
            r.actionDamageType = DAMAGE_TYPE_ELEMENTAL + (p.spellElement or 0)
            r.afflatusMisery = true
            r.updateEnmity = true
            if p.canMagicBurst then
                r.spellMsg = MSG_MAGIC_BURST_DAMAGE
                r.setSpellMsg = true
                r.triggerRoeBurst = true
            end
        end
        return r
    end

    it('nullified sets MAGIC_RESIST and returns 0', function()
        local r = execute({ nullified = true, base = 1000 })
        assert(r.nullified == true and r.final == 0 and r.spellMsg == MSG_MAGIC_RESIST)
    end)

    it('absorb path recovers HP message', function()
        local r = execute({ base = 100, mults = { -1 } })
        assert(r.absorbHP == true and r.final == 100 and r.spellMsg == MSG_MAGIC_RECOVERS_HP)
        assert(r.takeSpellDamage ~= true)
    end)

    it('absorb uses host addHP return when provided', function()
        local r = execute({ base = 100, mults = { -1 }, useAbsorbHealed = true, absorbHealed = 40 })
        assert(r.final == 40)
    end)

    it('damage path takeSpellDamage and elemental type', function()
        local r = execute({ base = 100, spellElement = 3 })
        assert(r.final == 100 and r.takeSpellDamage == true)
        assert(r.actionDamageType == DAMAGE_TYPE_ELEMENTAL + 3)
        assert(r.afflatusMisery == true and r.updateEnmity == true)
    end)

    it('magic burst message on damage path', function()
        local r = execute({ base = 50, canMagicBurst = true })
        assert(r.spellMsg == MSG_MAGIC_BURST_DAMAGE and r.triggerRoeBurst == true)
    end)

    it('phalanx reduces positive damage', function()
        local r = execute({ base = 100, phalanxMod = 30 })
        assert(r.final == 70)
    end)

    it('full mitigation zeroes takeSpellDamage', function()
        local r = execute({ base = 50, phalanxMod = 1000 })
        assert(r.final == 0 and r.takeSpellDamage ~= true)
    end)

    it('damage cap clamps positive final', function()
        local r = execute({ base = 500, capActive = true, capValue = 100 })
        assert(r.final == 100)
    end)

    it('absorb wins over burst message', function()
        local r = execute({ base = 80, mults = { -1 }, canMagicBurst = true })
        assert(r.spellMsg == MSG_MAGIC_RECOVERS_HP and r.triggerRoeBurst ~= true)
    end)
end)
