describe('Status effect list bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ job = xi.job.WHM, level = 99 })
    end)

    it('returns active effects in order and exposes live flag helpers', function()
        player:addStatusEffect(xi.effect.PROTECT, { power = 5, duration = 60, origin = player, flag = xi.effectFlag.DISPELABLE })
        player:addStatusEffect(xi.effect.SHELL, { power = 6, duration = 60, origin = player, flag = xi.effectFlag.ERASABLE })

        local effects = player:getStatusEffects()
        local protect = effects[#effects - 1]
        local shell = effects[#effects]

        assert(protect:getEffectType() == xi.effect.PROTECT, 'Protect was not the first appended status effect')
        assert(shell:getEffectType() == xi.effect.SHELL, 'Shell was not the second appended status effect')
        assert(protect:getPower() == 5, 'Protect power was not preserved')
        assert(shell:getPower() == 6, 'Shell power was not preserved')

        protect:addEffectFlag(xi.effectFlag.ERASABLE)
        shell:setEffectFlags(xi.effectFlag.DISPELABLE)
        shell:delEffectFlag(xi.effectFlag.DISPELABLE)
        shell:addEffectFlag(xi.effectFlag.NO_CANCEL)

        local protectAgain = player:getStatusEffect(xi.effect.PROTECT)
        local shellAgain = player:getStatusEffect(xi.effect.SHELL)

        assert(protectAgain:hasEffectFlag(xi.effectFlag.DISPELABLE), 'Protect lost its original flag')
        assert(protectAgain:hasEffectFlag(xi.effectFlag.ERASABLE), 'Protect did not receive the added flag')
        assert(shellAgain:getEffectFlags() == xi.effectFlag.NO_CANCEL, 'Shell did not retain live flag mutations')
        assert(not shellAgain:hasEffectFlag(xi.effectFlag.DISPELABLE), 'Shell kept a deleted flag')
    end)
end)
