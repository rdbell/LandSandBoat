-----------------------------------
-- Pure system tests for souleater self-damage inject half (slice 6091).
-----------------------------------

describe('souleater self-damage pure injects', function()
    local function souleaterBonus(p)
        if not p.hasEffect then
            return 0
        end
        return math.floor(p.hp * (0.1 + (p.souleaterEffect or 0) / 100 + (p.souleaterEffectII or 0) / 100))
    end

    local function souleaterSelfDamage(bonusDamage, stalwartSoulPercent)
        if bonusDamage <= 0 then
            return 0
        end
        return bonusDamage * (1 - stalwartSoulPercent / 100)
    end

    it('bonus and self-damage product', function()
        assert(souleaterBonus({ hasEffect = false, hp = 1000 }) == 0)
        local bonus = souleaterBonus({ hasEffect = true, hp = 1000 })
        assert(bonus == 100)
        assert(souleaterSelfDamage(bonus, 0) == 100)
        assert(souleaterSelfDamage(bonus, 20) == 80)
        assert(souleaterSelfDamage(0, 10) == 0)
    end)
end)
