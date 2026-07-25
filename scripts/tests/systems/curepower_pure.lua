-----------------------------------
-- Pure system tests for cure power dual-wire (slice 6718).
-- Calls production xi.magic pure exports.
-- Goldens match internal/curepower (0867).
-----------------------------------

require('scripts/globals/magic')

local m = xi.magic

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Cure power constants', function()
    it('pins potency caps and fixed mults', function()
        assert(m.curePotencyCap == 50)
        assert(m.curePotencyIICap == 30)
        assert(m.divineSealMult == 2)
        assert(almost(m.raptureBase, 1.5))
    end)
end)

describe('getCurePowerFromParams / Old', function()
    it('floor MND/VIT + skill', function()
        assert(m.getCurePowerFromParams({ mnd = 100, vit = 80, healingSkill = 200 }) == 270)
        assert(m.getCurePowerFromParams({ mnd = 5, vit = 7, healingSkill = 0 }) == 3)
        assert(m.getCurePowerFromParams({}) == 0)
    end)

    it('old formula 3*MND + VIT + 3*floor(skill/5)', function()
        assert(m.getCurePowerOldFromParams({ mnd = 100, vit = 80, healingSkill = 200 }) == 500)
        assert(m.getCurePowerOldFromParams({ mnd = 10, vit = 5, healingSkill = 4 }) == 35)
        assert(m.getCurePowerOldFromParams({ mnd = 0, vit = 0, healingSkill = 5 }) == 3)
    end)
end)

describe('getBaseCureFromParams / Old', function()
    it('ladder arithmetic', function()
        assert(almost(m.getBaseCureFromParams({
            power = 270, divisor = 1.5, constant = 100, basepower = 60,
        }), 240))
        assert(almost(m.getBaseCureFromParams({
            power = 60, divisor = 2, constant = 50, basepower = 60,
        }), 50))
        assert(almost(m.getBaseCureOldFromParams({
            power = 500, divisor = 2, constant = 60,
        }), 185))
        assert(almost(m.getBaseCureOldFromParams({
            power = 0, divisor = 1, constant = 10,
        }), 10))
    end)
end)

describe('getCureFinalFromParams', function()
    it('min cure and potency caps', function()
        local f, c = m.getCureFinalFromParams({
            baseCure = 10, minCure = 60, dayWeatherBonus = 1,
        })
        assert(f == 60 and not c)

        f, c = m.getCureFinalFromParams({
            baseCure = 100, minCure = 60, curePotency = 50, curePotencyII = 30,
            dayWeatherBonus = 1,
        })
        assert(f == 180 and not c)

        f = m.getCureFinalFromParams({
            baseCure = 100, curePotency = 80, curePotencyII = 40, dayWeatherBonus = 1,
        })
        assert(f == 180)

        f = m.getCureFinalFromParams({
            baseCure = 100, curePotency = 20, dayWeatherBonus = 1,
        })
        assert(f == 120)
    end)

    it('day/weather and Divine Seal product order', function()
        local f = m.getCureFinalFromParams({
            baseCure = 100, dayWeatherBonus = 1.1, hasDivineSeal = true,
        })
        assert(f == 220)

        f = m.getCureFinalFromParams({
            baseCure = 100, dayWeatherBonus = 0,
        })
        assert(f == 0)
    end)

    it('Rapture consume and BLU skip', function()
        local f, c = m.getCureFinalFromParams({
            baseCure = 100, dayWeatherBonus = 1, hasRapture = true,
        })
        assert(f == 150 and c)

        f, c = m.getCureFinalFromParams({
            baseCure = 100, dayWeatherBonus = 1, hasRapture = true, raptureAmountMod = 10,
        })
        assert(f == 160 and c)

        f, c = m.getCureFinalFromParams({
            baseCure = 100, dayWeatherBonus = 1, isBlueMagic = true,
            hasRapture = true, raptureAmountMod = 50,
        })
        assert(f == 100 and not c)
    end)

    it('full stepwise floor product', function()
        local f, c = m.getCureFinalFromParams({
            baseCure = 123.9, minCure = 100, curePotency = 25,
            dayWeatherBonus = 1.1, hasRapture = true, hasDivineSeal = true,
        })
        assert(f == 504 and c)
    end)
end)

describe('isValidHealTargetFromParams', function()
    it('allegiance and objType allowlist', function()
        assert(m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 1, targetObjType = xi.objType.PC,
        }))
        assert(m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 1, targetObjType = xi.objType.MOB,
        }))
        assert(m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 1, targetObjType = xi.objType.TRUST,
        }))
        assert(m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 1, targetObjType = xi.objType.FELLOW,
        }))
        assert(not m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 2, targetObjType = xi.objType.PC,
        }))
        assert(not m.isValidHealTargetFromParams({
            casterAllegiance = 1, targetAllegiance = 1, targetObjType = xi.objType.NPC,
        }))
    end)
end)
