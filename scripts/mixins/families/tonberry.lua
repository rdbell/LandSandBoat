require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.tonberry = xi.mix.tonberry or {}

xi.mix.tonberry.nextGrudgeKills = function(kills, memberZone, playerZone)
    if memberZone == playerZone and kills < 480 then
        return kills + 1
    end
    return nil
end

g_mixins.families.tonberry = function(tonberryMob)
    tonberryMob:addListener('DEATH', 'TONBERRY_DEATH', function(mob, player)
        if player then
            local alliance = player:getAlliance()
            for _, member in pairs(alliance) do
                local nextKills = xi.mix.tonberry.nextGrudgeKills(
                    member:getCharVar('EVERYONES_GRUDGE_KILLS'),
                    member:getZoneID(),
                    player:getZoneID())
                if nextKills then
                    member:setCharVar('EVERYONES_GRUDGE_KILLS', nextKills)
                end
            end
        end
    end)
end

return g_mixins.families.tonberry
