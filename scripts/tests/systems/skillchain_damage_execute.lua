-----------------------------------
-- Pure system tests for calculateSkillchainDamage full plan (slice 6107).
-- Source: scripts/globals/combat/skillchain.lua ~66–146
-----------------------------------

describe('calculateSkillchainDamage pure execute plan', function()
    local DAMAGE_TYPE_ELEMENTAL = 5
    local ELEMENT_FIRE = 1

    local chainMult = {
        [1] = { 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 },
    }

    local function product(p)
        if not p.hasEffect or (p.type or 0) == 0 then
            return { applied = false, damage = 0, element = 0, consumeSengikori = false }
        end
        if (p.level or 0) < 1 or (p.level or 0) > 4 then
            return { applied = false, damage = 0, element = 0, consumeSengikori = false }
        end
        if (p.count or 0) < 1 or (p.count or 0) > 6 then
            return { applied = false, damage = 0, element = 0, consumeSengikori = false }
        end
        if (p.element or 0) == 0 then
            return { applied = false, damage = 0, element = 0, consumeSengikori = false }
        end
        if p.nullified then
            return { applied = false, damage = 0, element = p.element, consumeSengikori = false }
        end
        local final = math.abs(p.base or 0)
        final = math.floor(final * chainMult[p.level][p.count])
        local mults = p.mults or {}
        for i = 1, #mults do
            final = math.floor(final * mults[i])
        end
        return {
            applied = true,
            damage = final,
            element = p.element,
            consumeSengikori = true,
        }
    end

    local function clampSC(damage)
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
        return clampSC(damage)
    end

    local function applyCap(damage, capActive, capValue)
        if not capActive or damage <= 0 then return damage end
        if damage > capValue then return capValue end
        return damage
    end

    local function execute(p)
        local prod = product(p)
        if not prod.applied then
            return {
                applied = false, final = prod.damage, element = prod.element,
                consumeSengikori = prod.consumeSengikori, takeDamage = false, healTarget = false,
            }
        end
        local damage = prod.damage
        if damage > 0 then
            damage = mitigation(damage, p.phalanxMod, p.ofaPower, p.stoneskin, p.ofaActive)
            damage = applyCap(damage, p.capActive, p.capValue or 0)
            return {
                applied = true, final = damage, element = prod.element,
                consumeSengikori = true,
                takeDamage = damage > 0,
                actionDamageType = DAMAGE_TYPE_ELEMENTAL + prod.element,
                healTarget = false,
            }
        end
        if damage < 0 then
            return {
                applied = true, final = damage, element = prod.element,
                consumeSengikori = true, takeDamage = false,
                healTarget = true, healAmount = -damage,
            }
        end
        return {
            applied = true, final = 0, element = prod.element,
            consumeSengikori = true, takeDamage = false, healTarget = false,
        }
    end

    local function base(p)
        p = p or {}
        p.hasEffect = true
        p.type = p.type or 1
        p.level = p.level or 1
        p.count = p.count or 1
        p.element = p.element or ELEMENT_FIRE
        p.base = p.base or 100
        return p
    end

    it('early out without effect', function()
        local r = execute({ hasEffect = false, base = 100 })
        assert(r.applied == false and r.final == 0)
    end)

    it('nullified keeps element without sengikori', function()
        local r = execute(base({ nullified = true }))
        assert(r.applied == false and r.element == ELEMENT_FIRE and r.consumeSengikori == false)
    end)

    it('level1 count1 multiplies 0.5', function()
        local r = execute(base({ base = 100 }))
        assert(r.final == 50 and r.takeDamage == true)
        assert(r.actionDamageType == DAMAGE_TYPE_ELEMENTAL + ELEMENT_FIRE)
    end)

    it('phalanx reduces positive damage', function()
        local r = execute(base({ base = 100, phalanxMod = 20 }))
        assert(r.final == 30)
    end)

    it('full mitigation zeroes takeDamage', function()
        local r = execute(base({ base = 100, phalanxMod = 1000 }))
        assert(r.final == 0 and r.takeDamage == false)
    end)

    it('absorb mult heals target', function()
        local r = execute(base({ base = 100, mults = { -1 } }))
        assert(r.healTarget == true and r.healAmount == 50 and r.final == -50)
        assert(r.takeDamage == false)
    end)

    it('damage cap clamps positive final', function()
        local r = execute(base({ base = 1000, capActive = true, capValue = 100 }))
        -- product 500 → cap 100
        assert(r.final == 100)
    end)
end)
