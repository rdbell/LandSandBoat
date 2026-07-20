require('scripts/actions/mobskills/sentinel')
describe('Sentinel mob skill', function()
    it('adds enmity and sets NONE message', function()
        local sentinel = require('scripts/actions/mobskills/sentinel')
        local enmity, message = nil, nil
        local mob = {}
        local target = { addEnmity = function(_, actor, ce, ve) enmity = { actor, ce, ve } end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(sentinel.onMobSkillCheck(target, mob, skill) == 0)
        local ret = sentinel.onMobWeaponSkill(mob, target, skill, {})
        assert(ret == nil or ret == 0)
        assert(enmity[1] == mob and enmity[2] == 1 and enmity[3] == 1800)
        assert(message == xi.msg.basic.NONE)
    end)
end)
