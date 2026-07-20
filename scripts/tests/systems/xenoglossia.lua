require('scripts/actions/mobskills/xenoglossia')
describe('Xenoglossia mob skill', function()
    it('applies UFASTCAST 150, registers MAGIC_START cleanup, and messages USES', function()
        local skill = require('scripts/actions/mobskills/xenoglossia')
        local mod, listener, message = nil, nil, nil
        local mob = {
            addMod = function(_, m, amount) mod = { m, amount } end,
            addListener = function(_, event, name, fn) listener = { event, name, fn } end,
            delMod = function(_, m, amount) listener.del = { m, amount } end,
            removeListener = function(_, name) listener.removed = name end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(mod[1] == xi.mod.UFASTCAST and mod[2] == 150)
        assert(listener[1] == 'MAGIC_START' and listener[2] == 'XENOGLOSSIA_MAGIC_START')
        assert(message == xi.msg.basic.USES)
        -- fire listener cleanup
        listener[3](mob, {}, {}, {})
        assert(listener.del[1] == xi.mod.UFASTCAST and listener.del[2] == 150)
        assert(listener.removed == 'XENOGLOSSIA_MAGIC_START')
    end)
end)
