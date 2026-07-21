-----------------------------------
-- Pure system tests for doAbsorbTPSpell full plan (slice 6119).
-- Source: scripts/globals/spells/absorb_spell.lua ~217–270
-----------------------------------

describe('doAbsorbTPSpell pure plan', function()
    local MSG_MAGIC_RESIST = 85
    local MSG_NO_EFFECT = 283
    local MSG_MAGIC_ABSORB_TP = 454

    local function use(p)
        if p.absorbedOrNull then
            return { returnDamage = 0, msg = MSG_MAGIC_RESIST, transferTP = false }
        end
        if (p.targetTP or 0) == 0 then
            return { returnDamage = 0, msg = MSG_NO_EFFECT, transferTP = false }
        end
        local dmg = p.finalDamage or 0
        return {
            returnDamage = dmg,
            msg = MSG_MAGIC_ABSORB_TP,
            transferTP = true,
            damage = dmg,
        }
    end

    it('absorb or null sets MAGIC_RESIST', function()
        local r = use({ absorbedOrNull = true, targetTP = 1000, finalDamage = 300 })
        assert(r.returnDamage == 0 and r.msg == MSG_MAGIC_RESIST)
        assert(r.transferTP ~= true)
    end)

    it('zero target TP sets NO_EFFECT', function()
        local r = use({ targetTP = 0, finalDamage = 100 })
        assert(r.returnDamage == 0 and r.msg == MSG_NO_EFFECT)
    end)

    it('success transfers TP and returns damage', function()
        local r = use({ targetTP = 2000, finalDamage = 600 })
        assert(r.returnDamage == 600 and r.msg == MSG_MAGIC_ABSORB_TP)
        assert(r.transferTP == true and r.damage == 600)
    end)

    it('zero final damage still sets MAGIC_ABSORB_TP when TP present', function()
        -- product can be 0 after resist; LSB still sets absorb msg and transfers 0
        local r = use({ targetTP = 50, finalDamage = 0 })
        assert(r.returnDamage == 0 and r.msg == MSG_MAGIC_ABSORB_TP and r.transferTP == true)
    end)
end)
