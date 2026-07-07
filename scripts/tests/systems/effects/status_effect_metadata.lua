describe('Status effect metadata bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ job = xi.job.WHM, level = 99 })
    end)

    it('returns constructor metadata and applies live metadata setters', function()
        player:addStatusEffect(xi.effect.PROTECT, {
            power           = 5,
            tick            = 3,
            duration        = 60,
            origin          = player,
            icon            = xi.effect.PROTECT,
            subType         = 22,
            subPower        = 7,
            subIcon         = xi.effect.SHELL,
            tier            = 2,
            sourceType      = xi.effectSourceType.EQUIPPED_ITEM,
            sourceTypeParam = 1234,
            slot            = 4,
        })

        local effect = player:getStatusEffect(xi.effect.PROTECT)
        assert(effect:getDuration() == 60000, 'Duration was not returned in milliseconds')
        assert(effect:getTick() == 3000, 'Tick was not returned in milliseconds')
        assert(effect:getIcon() == xi.effect.PROTECT, 'Icon was not preserved')
        assert(effect:getSubIcon() == xi.effect.SHELL, 'Sub-icon was not preserved')
        assert(effect:getSubType() == 22, 'Sub-type was not preserved')
        assert(effect:getSubPower() == 7, 'Sub-power was not preserved')
        assert(effect:getTier() == 2, 'Tier was not preserved')
        assert(effect:getEffectSlot() == 4, 'Effect slot was not preserved')
        assert(effect:getSourceType() == xi.effectSourceType.EQUIPPED_ITEM, 'Source type was not preserved')
        assert(effect:getSourceTypeParam() == 1234, 'Source parameter was not preserved')
        assert(effect:getOriginID() == player:getID(), 'Origin ID was not preserved')
        assert(effect:getTimeRemaining() <= effect:getDuration(), 'Remaining time exceeded duration')

        effect:setDuration(90000)
        effect:setTick(6000)
        effect:setIcon(xi.effect.SHELL)
        effect:setSubIcon(xi.effect.PROTECT)
        effect:setSource(xi.effectSourceType.FOOD, 5678)
        effect:setOriginID(4321)
        effect:setEffectSlot(6)

        assert(player:getStatusEffectBySource(xi.effect.PROTECT, xi.effectSourceType.EQUIPPED_ITEM, 1234) == nil, 'Old source still found the effect after setSource')
        assert(player:getStatusEffectBySource(xi.effect.PROTECT, xi.effectSourceType.FOOD, 5678) ~= nil, 'New source did not find the effect after setSource')
        assert(effect:getDuration() == 90000, 'Duration setter did not update the effect')
        assert(effect:getTick() == 6000, 'Tick setter did not update the effect')
        assert(effect:getIcon() == xi.effect.SHELL, 'Icon setter did not update the effect')
        assert(effect:getSubIcon() == xi.effect.PROTECT, 'Sub-icon setter did not update the effect')
        assert(effect:getSourceType() == xi.effectSourceType.FOOD, 'Source type setter did not update the effect')
        assert(effect:getSourceTypeParam() == 5678, 'Source parameter setter did not update the effect')
        assert(effect:getOriginID() == 4321, 'Origin setter did not update the effect')
        assert(effect:getEffectSlot() == 6, 'Effect slot setter did not update the effect')
    end)
end)
