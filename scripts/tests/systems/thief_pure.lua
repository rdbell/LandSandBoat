-----------------------------------
-- Pure system tests for Thief dual-wire (slice 6745).
-- Calls production xi.job_utils.thief pure exports.
-- Goldens match internal/thief (0901).
-----------------------------------

require('scripts/globals/job_utils/thief')

local t = xi.job_utils.thief

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Thief pure pins', function()
    it('bases and catalogs', function()
        assert(t.stealChanceBase == 50 and t.stealModScale == 2)
        assert(t.mugChanceBase == 90 and almost(t.mugHPStealScale, 0.05))
        assert(t.mugGilDivBase == 8)
        assert(t.despoilDebuffDefaultPower == 10)
        assert(t.despoilSlowBase == 1500 and t.despoilSlowMin == 750 and t.despoilSlowMax == 3000)
        assert(t.bullyBasePower == 15 and t.perfectDodgeBaseDuration == 30)
        assert(t.fleeBaseDuration == 30 and t.fleePower == 10000)
        assert(t.sneakAttackDuration == 60 and t.trickAttackDuration == 60)
        assert(t.msgCannotOnThatTarg == 155)
        assert(t.oneHourRecastSecondsPerMod == 60)

        assert(t.despoilDebuffs and #t.despoilDebuffs == 7)
        assert(t.stealableSPEffects and #t.stealableSPEffects == 13)
        assert(t.despoilDebuffs[1] == xi.effect.EVASION_DOWN)
        assert(t.despoilDebuffs[7] == xi.effect.SLOW)
    end)
end)

describe('checks chance mug', function()
    it('gates chance mug products', function()
        local msg, ok = t.checkAccompliceFromParams({ isSelf = false, isPC = true })
        assert(ok and msg == 0)
        msg, ok = t.checkAccompliceFromParams({ isSelf = true, isPC = true })
        assert(not ok and msg == 155)
        msg, ok = t.checkAccompliceFromParams({ isSelf = false, isPC = false })
        assert(not ok and msg == 155)
        msg, ok = t.checkCollaboratorFromParams({ isSelf = false, isPC = true })
        assert(ok and msg == 0)

        assert(t.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(t.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(t.oneHourRecastFromParams({ abilityRecast = 60, oneHourRecastMod = 5 }) == 0)

        assert(t.chanceFromParams({ mod = 0, level = 75, targetMainLvl = 75 }) == 50)
        assert(t.chanceFromParams({ mod = 5, level = 99, targetMainLvl = 90 }) == 69)
        assert(t.mugChanceFromParams({ thfLevel = 75, targetMainLvl = 70 }) == 95)

        assert(almost(t.mugHPStealFromParams({ agi = 100, dex = 50, mugJP = 2 }), 15))
        assert(almost(t.mugHPStealFromParams({ agi = 200, dex = 200, mugJP = 0 }), 0))

        assert(t.mugGilFromParams({ fatpurse = 800, purse = 1000, divRoll0to8 = 0, mugEffectMod = 0 }) == 100)
        assert(t.mugGilFromParams({ fatpurse = 800, purse = 1000, divRoll0to8 = 8, mugEffectMod = 0 }) == 50)
        assert(t.mugGilFromParams({ fatpurse = 800, purse = 40, divRoll0to8 = 0, mugEffectMod = 0 }) == 40)
        assert(t.mugGilFromParams({ fatpurse = 800, purse = 1000, divRoll0to8 = 0, mugEffectMod = 1 }) == 200)
        assert(t.mugGilFromParams({ fatpurse = 0, purse = 100, divRoll0to8 = 0, mugEffectMod = 0 }) == 0)
        assert(t.mugGilFromParams({ fatpurse = 100, purse = 1000, divRoll0to8 = 0, mugEffectMod = 0 }) == 12)
    end)
end)

describe('despoil conspirator charge bully durations', function()
    it('products', function()
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.ATTACK_DOWN }) == 20)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.DEFENSE_DOWN }) == 30)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.MAGIC_ATK_DOWN }) == 10)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.MAGIC_DEF_DOWN }) == 20)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.EVASION_DOWN }) == 30)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.ACCURACY_DOWN }) == 20)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.SLOW, playerMND = 50, targetMND = 50 }) == 1500)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.SLOW, playerMND = 200, targetMND = 100 }) == 1700)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.SLOW, playerMND = 0, targetMND = 100 }) == 1400)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.SLOW, playerMND = 0, targetMND = 1000 }) == 750)
        assert(t.despoilDebuffPowerFromParams({ debuff = xi.effect.SLOW, playerMND = 1100, targetMND = 100 }) == 3000)

        local sb, acc = t.conspiratorPowersFromParams(0)
        assert(sb == 0 and acc == 0)
        sb, acc = t.conspiratorPowersFromParams(1)
        assert(sb == 20 and acc == 15)
        sb, acc = t.conspiratorPowersFromParams(6)
        assert(sb == 50 and acc == 25)
        sb, acc = t.conspiratorPowersFromParams(18)
        assert(sb == 50 and acc == 49)

        sb, acc = t.conspiratorScaledFromParams({ subtleBlow = 20, accuracy = 15, scale = 2 })
        assert(sb == 40 and acc == 30)

        assert(t.assassinsChargePowerFromParams(5) == 0)
        assert(t.assassinsChargePowerFromParams(10) == 5)
        assert(almost(t.assassinsChargeSubPowerFromParams({
            merits = 10, augmentsAssassinsCharge = true,
        }), 2))
        assert(almost(t.assassinsChargeSubPowerFromParams({
            merits = 10, augmentsAssassinsCharge = false,
        }), 0))

        assert(t.bullyPowerFromParams(5) == 20)
        assert(t.perfectDodgeDurationFromParams(10) == 40)
        assert(t.fleeDurationFromParams(5) == 35)

        local p = t.sneakAttackFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = t.trickAttackFromParams()
        assert(p.power == 1 and p.duration == 60)
    end)
end)
