require('scripts/actions/mobskills/barrage')
describe('Barrage mob skill', function()
    it('sets JOBABILITY_FINISH, adds Barrage 60s, messages USES', function()
        local skill = require('scripts/actions/mobskills/barrage')
        local category, added, message = nil, nil, nil
        local action = { setCategory = function(_, v) category = v end }
        local mob = {
            addStatusEffect = function(self, effect, opts)
                added = { effect, opts }
            end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, action) == xi.effect.BARRAGE)
        assert(category == xi.action.category.JOBABILITY_FINISH)
        assert(added[1] == xi.effect.BARRAGE and added[2].duration == 60 and added[2].origin == mob)
        assert(message == xi.msg.basic.USES)
    end)
end)
