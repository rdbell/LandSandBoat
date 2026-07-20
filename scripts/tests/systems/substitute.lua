require('scripts/actions/mobskills/substitute')
describe('Substitute mob skill', function()
    it('applies TELEPORT escape status with NONE message', function()
        local sub = require('scripts/actions/mobskills/substitute')
        local message, effect = nil, nil
        local target = {
            addStatusEffect = function(_, e, opts) effect = { e, opts } end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(sub.onMobSkillCheck(target, {}, skill) == 0)
        sub.onMobWeaponSkill({}, target, skill, {})
        assert(message == xi.msg.basic.NONE)
        assert(effect[1] == xi.effect.TELEPORT and effect[2].power == xi.teleport.id.ESCAPE and effect[2].duration == 3 and effect[2].icon == 0)
    end)
end)
