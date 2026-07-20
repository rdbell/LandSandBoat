require('scripts/actions/mobskills/emetic_discharge')

describe('Emetic Discharge mob skill', function()
    it('transfers removable ailments only when its shadow removal succeeds', function()
        local discharge = require('scripts/actions/mobskills/emetic_discharge')
        local takeShadows = utils.takeShadows
        local messages, added, removed, shadowCalls = {}, {}, {}, {}
        local flash = xi.effect.FLASH
        local burn = xi.effect.BURN
        local effects = {
            [flash] = { getPower = function() return 17 end, getDuration = function() return 45 end, getTick = function() return 3 end },
            [burn] = { getPower = function() return 11 end, getDuration = function() return 30 end, getTick = function() return 5 end },
        }
        local mob = {
            getStatusEffect = function(_, effect) return effects[effect] end,
            delStatusEffect = function(_, effect) table.insert(removed, effect) end,
        }
        local target = {
            addStatusEffect = function(_, effect, options) table.insert(added, { effect, options }) end,
        }
        local skill = { setMsg = function(_, message) table.insert(messages, message) end }

        local random = math.random
        math.random = function(low, high) assert(low == 2 and high == 3); return 2 end
        utils.takeShadows = function(_, minimum, maximum) table.insert(shadowCalls, { minimum, maximum }); return 0 end
        assert(discharge.onMobSkillCheck(target, mob, skill) == 0)
        assert(discharge.onMobWeaponSkill(mob, target, skill, {}) == 0)
        assert(#added == 0 and #removed == 0 and messages[1] == xi.msg.basic.SKILL_NO_EFFECT)

        utils.takeShadows = function(_, minimum, maximum) table.insert(shadowCalls, { minimum, maximum }); return 1 end
        assert(discharge.onMobWeaponSkill(mob, target, skill, {}) == 0)

        utils.takeShadows, math.random = takeShadows, random

        assert(shadowCalls[1][1] == 1 and shadowCalls[1][2] == 2 and shadowCalls[2][1] == 1 and shadowCalls[2][2] == 2)
        assert(#added == 2 and #removed == 2 and added[1][1] == flash and added[2][1] == burn and removed[1] == flash and removed[2] == burn)
        assert(added[1][2].power == 17 and added[1][2].duration == 45 and added[1][2].tick == 3 and added[1][2].origin == mob)
        assert(added[2][2].power == 11 and added[2][2].duration == 30 and added[2][2].tick == 5 and added[2][2].origin == mob)
        assert(messages[2] == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
