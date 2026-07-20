require('scripts/actions/mobskills/pyric_bulwark')
describe('Pyric Bulwark mob skill', function()
    it('allows animation sub <= 1 and applies Physical Shield with SKILL_GAIN_EFFECT', function()
        local bulwark = require('scripts/actions/mobskills/pyric_bulwark')
        local animSub, effect, message = 0, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            addStatusEffect = function(_, effectID, opts) effect = { effectID, opts } end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        animSub = 2; assert(bulwark.onMobSkillCheck({}, mob, skill) == 1)
        animSub = 1; assert(bulwark.onMobSkillCheck({}, mob, skill) == 0)
        assert(bulwark.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PHYSICAL_SHIELD)
        assert(effect[1] == xi.effect.PHYSICAL_SHIELD and effect[2].power == 1 and effect[2].duration == 45)
        assert(message == xi.msg.basic.SKILL_GAIN_EFFECT)
    end)
end)
