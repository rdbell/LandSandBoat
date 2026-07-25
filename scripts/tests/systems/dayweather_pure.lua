-----------------------------------
-- Pure system tests for day/weather damage mult dual-wire (slice 6710).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/dayweather (0855).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local el = xi.element
local weather = xi.weather

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Day/weather mult constants', function()
    it('pins steps and clamp', function()
        assert(dmg.dayWeatherMultMin == 0)
        assert(dmg.dayWeatherMultMax == 1.4)
        assert(almost(dmg.dayWeatherSingleStep, 0.10))
        assert(almost(dmg.dayWeatherDoubleStep, 0.25))
        assert(almost(dmg.dayWeatherDayStep, 0.10))
        assert(almost(dmg.dayWeatherIridescenceStep, 0.05))
        assert(dmg.dayWeatherProcChance == 33)
    end)
end)

describe('calculateDayAndWeatherFromParams', function()
    it('invalid element returns 1', function()
        assert(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.NONE, alwaysApply = true, weather = weather.HOT_SPELL,
        }) == 1)
    end)

    it('no proc stays neutral (day-nuke still applies)', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HOT_SPELL, dayElement = el.ICE,
        }), 1.0))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.WATER, weather = 1, dayElement = el.WATER, dayNukeBonus = 15,
        }), 1.15))
    end)

    it('single and double weather with iridescence', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HOT_SPELL, dayElement = el.ICE,
            alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.10))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HEAT_WAVE, dayElement = el.ICE,
            alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.25))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HOT_SPELL, dayElement = el.ICE,
            alwaysApply = true, iridescence = 1,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.15))
    end)

    it('matching day and day-nuke fire..water only', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = 1, dayElement = el.FIRE, alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.10))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = 1, dayElement = el.FIRE,
            alwaysApply = true, dayNukeBonus = 10,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.20))
        -- light matching day + dayNuke should NOT get day-nuke bonus
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.LIGHT, weather = 1, dayElement = el.LIGHT,
            alwaysApply = true, dayNukeBonus = 50,
            associatedSingleWeather = weather.AURORAS,
            associatedDoubleWeather = weather.STELLAR_GLARE,
        }), 1.10))
    end)

    it('penalties on opposite weather/day', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.RAIN, dayElement = el.ICE,
            alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
            oppositeSingleWeather = weather.RAIN,
            oppositeDoubleWeather = weather.SQUALL,
            elementWeakness = el.WATER,
        }), 0.90))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.SQUALL, dayElement = el.ICE,
            alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
            oppositeSingleWeather = weather.RAIN,
            oppositeDoubleWeather = weather.SQUALL,
            elementWeakness = el.WATER,
        }), 0.75))
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = 1, dayElement = el.WATER,
            alwaysApply = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
            oppositeSingleWeather = weather.RAIN,
            oppositeDoubleWeather = weather.SQUALL,
            elementWeakness = el.WATER,
        }), 0.90))
    end)

    it('elemental obi forces bonuses only', function()
        -- weak day would penalty under alwaysApply; obi alone does not
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = 1, dayElement = el.WATER,
            forceElementBonus = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
            oppositeSingleWeather = weather.RAIN,
            oppositeDoubleWeather = weather.SQUALL,
            elementWeakness = el.WATER,
        }), 1.0))
        -- matching day under obi gets bonus
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = 1, dayElement = el.FIRE,
            forceElementBonus = true,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.10))
    end)

    it('twilight cape proc bonus on strong match', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HOT_SPELL, dayElement = el.ICE,
            alwaysApply = true, dayWeatherProcBonus = 5,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.15))
    end)

    it('clamps to 1.4', function()
        assert(almost(dmg.calculateDayAndWeatherFromParams({
            spellElement = el.FIRE, weather = weather.HEAT_WAVE, dayElement = el.FIRE,
            alwaysApply = true, iridescence = 2, dayWeatherProcBonus = 20, dayNukeBonus = 20,
            associatedSingleWeather = weather.HOT_SPELL,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }), 1.4))
    end)
end)
