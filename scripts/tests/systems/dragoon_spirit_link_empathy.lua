require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link Empathy bonus', function()
    local function apply(merits, effects, existingEffects)
        local awardedExp
        local copied = {}
        local removed = {}
        local originalAddWyvernExp = xi.job_utils.dragoon.addWyvernExp
        xi.job_utils.dragoon.addWyvernExp = function(_, exp) awardedExp = exp end

        local player = {
            getMerit = function() return merits end,
            getStatusEffects = function() return effects end,
        }
        local wyvern = {
            hasStatusEffect = function(_, effect) return existingEffects[effect] end,
            delStatusEffectSilent = function(_, effect) table.insert(removed, effect) end,
            copyStatusEffect = function(_, effect) table.insert(copied, effect:getEffectType()) end,
        }

        xi.job_utils.dragoon.applyEmpathyBonus(player, wyvern)
        xi.job_utils.dragoon.addWyvernExp = originalAddWyvernExp
        return awardedExp, copied, removed
    end

    local function effect(id, empathy)
        return {
            getEffectType = function() return id end,
            hasEffectFlag = function() return empathy end,
        }
    end

    it('awards 200 EXP per merit and copies each eligible effect up to the merit count', function()
        local exp, copied, removed = apply(2, { effect(10, true), effect(20, false), effect(30, true) }, { [10] = true })
        assert(exp == 400)
        assert(#copied == 2 and copied[1] == 10 and copied[2] == 30)
        assert(#removed == 1 and removed[1] == 10)
    end)

    it('limits copies when fewer eligible effects exist than merits', function()
        local exp, copied, removed = apply(3, { effect(10, false), effect(30, true) }, {})
        assert(exp == 600)
        assert(#copied == 1 and copied[1] == 30)
        assert(#removed == 0)
    end)
end)
