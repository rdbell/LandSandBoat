-----------------------------------
-- Pure system tests for element_tables.lua catalog and getters.
-----------------------------------

describe('Element data table pure plans', function()
    it('weakness and strength cycle', function()
        assert(xi.data.element.getElementWeakness(xi.element.FIRE) == xi.element.WATER)
        assert(xi.data.element.getElementStrength(xi.element.FIRE) == xi.element.ICE)
        for e = xi.element.FIRE, xi.element.DARK do
            local w = xi.data.element.getElementWeakness(e)
            assert(xi.data.element.getElementStrength(w) == e)
        end
        assert(xi.data.element.getElementWeakness(0) == 0)
        assert(xi.data.element.getElementStrength(9) == 0)
    end)

    it('day and weather associations reverse lookup', function()
        assert(xi.data.element.getAssociatedDay(xi.element.FIRE) == xi.day.FIRESDAY)
        assert(xi.data.element.getDayElement(xi.day.FIRESDAY) == xi.element.FIRE)
        assert(xi.data.element.getOppositeDay(xi.element.FIRE) == xi.data.element.getAssociatedDay(xi.element.WATER))
        assert(xi.data.element.getAssociatedSingleWeather(xi.element.FIRE) == xi.weather.HOT_SPELL)
        assert(xi.data.element.getAssociatedDoubleWeather(xi.element.FIRE) == xi.weather.HEAT_WAVE)
        assert(xi.data.element.getWeatherElement(xi.weather.HOT_SPELL) == xi.element.FIRE)
        assert(xi.data.element.getWeatherElement(xi.weather.HEAT_WAVE) == xi.element.FIRE)
    end)

    it('elemental mod getters match catalog columns', function()
        assert(xi.data.element.getElementalSDTModifier(xi.element.FIRE) == xi.mod.FIRE_SDT)
        assert(xi.data.element.getElementalMABModifier(xi.element.FIRE) == xi.mod.FIRE_MAB)
        assert(xi.data.element.getElementalStaffModifier(xi.element.FIRE) == xi.mod.FIRE_STAFF_BONUS)
        assert(xi.data.element.getElementalNullificationModifier(xi.element.FIRE) == xi.mod.FIRE_NULL)
        assert(xi.data.element.getElementalAbsorptionModifier(xi.element.FIRE) == xi.mod.FIRE_ABSORB)
        assert(xi.data.element.getElementalMACCModifier(xi.element.FIRE) == xi.mod.FIRE_MACC)
        assert(xi.data.element.getElementalMEVAModifier(xi.element.FIRE) == xi.mod.FIRE_MEVA)
        assert(xi.data.element.getElementalFTPModifier(xi.element.FIRE) == xi.mod.FIRE_FTP_BONUS)
        assert(xi.data.element.getForcedDayOrWeatherBonusModifier(xi.element.FIRE) == xi.mod.FORCE_FIRE_DWBONUS)
        assert(xi.data.element.getElementalResistanceRankModifier(xi.element.FIRE) == xi.mod.FIRE_RES_RANK)
        -- invalid
        assert(xi.data.element.getElementalMABModifier(0) == 0)
    end)

    it('barspell and merit getters', function()
        assert(xi.data.element.getAssociatedBarspellEffect(xi.element.FIRE) == xi.effect.BARFIRE)
        assert(xi.data.element.getAssociatedBarspellEffect(xi.element.LIGHT) == 0)
        assert(xi.data.element.getElementalPotencyMerit(xi.element.FIRE) == xi.merit.FIRE_MAGIC_POTENCY)
        assert(xi.data.element.getElementalAccuracyMerit(xi.element.FIRE) == xi.merit.FIRE_MAGIC_ACCURACY)
        assert(xi.data.element.getElementalPotencyMerit(xi.element.THUNDER) == xi.merit.LIGHTNING_MAGIC_POTENCY)
        assert(xi.data.element.getElementalPotencyMerit(xi.element.DARK) == 0)
    end)
end)
