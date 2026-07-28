require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.gear = xi.mix.gear or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.gear.stageForHPP = function(hpp, dropFirst, dropSecond)
    if hpp >= dropSecond and hpp <= dropFirst then
        return 1
    elseif hpp <= dropSecond then
        return 2
    elseif hpp > dropFirst then
        return 0
    end

    return nil
end

xi.mix.gear.shouldChangeStage = function(currentStage, targetStage)
    return targetStage ~= nil and currentStage ~= targetStage
end

xi.mix.gear.skillListForStage = function(stage)
    if stage == nil then
        return nil
    end

    return 150 + stage
end

xi.mix.gear.shouldDropDefense = function(stage, alreadyDropped)
    return (stage == 1 or stage == 2) and alreadyDropped == 0
end

g_mixins.families.gear = function(gearMob)
    gearMob:addListener('SPAWN', 'TRIPLE_GEAR_DROP', function(mob)
        -- Setup Triple Gears losing gears
        if mob:getSpecies() == xi.mobSpecies.TRIPLE_GEAR then
            mob:setLocalVar('gearDrop1', math.random(45, 60))
            mob:setLocalVar('gearDrop2', math.random(35, 20))
        end
    end)

    gearMob:addListener('COMBAT_TICK', 'GEARS_CTICK', function(mob)
        -- Triple Gears only
        if mob:getSpecies() == xi.mobSpecies.TRIPLE_GEAR then
            local mobHPP     = mob:getHPP()
            local tripleGear = 0
            local doubleGear = 1
            local singleGear = 2
            local dropFirst  = mob:getLocalVar('gearDrop1')
            local dropSecond = mob:getLocalVar('gearDrop2')

            local stage = xi.mix.gear.stageForHPP(mobHPP, dropFirst, dropSecond)

            if stage == doubleGear then
                if xi.mix.gear.shouldChangeStage(mob:getAnimationSub(), stage) then
                    mob:setAnimationSub(doubleGear)
                    mob:setMobMod(xi.mobMod.SKILL_LIST, xi.mix.gear.skillListForStage(stage))
                end

                if xi.mix.gear.shouldDropDefense(stage, mob:getLocalVar('Def1')) then
                    mob:delMod(xi.mod.MDEF, 10)
                    mob:delMod(xi.mod.DEF, 20)
                    mob:setLocalVar('Def1', 1)
                end
            elseif stage == singleGear then
                if xi.mix.gear.shouldChangeStage(mob:getAnimationSub(), stage) then
                    mob:setAnimationSub(singleGear)
                    mob:setMobMod(xi.mobMod.SKILL_LIST, xi.mix.gear.skillListForStage(stage))
                end

                if xi.mix.gear.shouldDropDefense(stage, mob:getLocalVar('Def2')) then
                    mob:delMod(xi.mod.MDEF, 10)
                    mob:delMod(xi.mod.DEF, 20)
                    mob:setLocalVar('Def2', 1)
                end
            elseif stage == tripleGear then
                if xi.mix.gear.shouldChangeStage(mob:getAnimationSub(), stage) then
                    mob:setAnimationSub(tripleGear)
                    mob:setMobMod(xi.mobMod.SKILL_LIST, xi.mix.gear.skillListForStage(stage))
                end
            end
        end
    end)
end

return g_mixins.families.gear
