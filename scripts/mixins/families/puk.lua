-----------------------------------
-- Puk family mixin
-----------------------------------
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.puk = xi.mix.puk or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}
-----------------------------------

local elementTable =
{
    [xi.day.FIRESDAY    ] = xi.damageType.FIRE,
    [xi.day.EARTHSDAY   ] = xi.damageType.EARTH,
    [xi.day.WATERSDAY   ] = xi.damageType.WATER,
    [xi.day.WINDSDAY    ] = xi.damageType.WIND,
    [xi.day.ICEDAY      ] = xi.damageType.ICE,
    [xi.day.LIGHTNINGDAY] = xi.damageType.THUNDER,
    [xi.day.LIGHTSDAY   ] = xi.damageType.LIGHT,
    [xi.day.DARKSDAY    ] = xi.damageType.DARK,
}

xi.mix.puk.damageTypeForDay = function(day)
    return elementTable[day]
end

xi.mix.puk.shouldGainTP = function(day, damageType)
    return damageType == xi.mix.puk.damageTypeForDay(day)
end

g_mixins.families.puk = function(mob)
    mob:addListener('TAKE_DAMAGE', 'PUK_TAKE_DAMAGE', function(puk, amount, attacker, attackType, damageType)
        -- If the element corresponding to the elemental day of the in-game Vana'diel week is used on a Puk, it will get 100% TP instantly.
        if xi.mix.puk.shouldGainTP(VanadielDayOfTheWeek(), damageType) then
            puk:addTP(1000)
        end
    end)
end

return g_mixins.families.puk
