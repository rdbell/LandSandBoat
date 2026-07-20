require('scripts/actions/mobskills/spider_web')
describe('Spider Web mob skill', function()
    it('executes Slow status effect table via combat action host', function()
        local web = require('scripts/actions/mobskills/spider_web')
        local exec = xi.combat.action.executeMobskillStatusEffect
        local args, ret = nil, 99
        xi.combat.action.executeMobskillStatusEffect = function(mob, target, skill, effectTable, opts)
            args = { effectTable, opts }
            return ret
        end
        assert(web.onMobSkillCheck({}, {}, {}) == 0)
        assert(web.onMobWeaponSkill({}, {}, {}, {}) == 99)
        xi.combat.action.executeMobskillStatusEffect = exec
        assert(args[1][1].effectId == xi.effect.SLOW and args[1][1].power == 3000)
        assert(args[1][1].duration == 90 and args[1][1].tier == 8)
    end)
end)
