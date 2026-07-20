require('scripts/actions/mobskills/providence')
describe('Providence mob skill', function()
    it('gates on local var and switches spell list with USES', function()
        local skill = require('scripts/actions/mobskills/providence')
        local msg, vars, spellList, listener = nil, {}, nil, nil
        local mob = {
            getLocalVar = function(_, k) return vars[k] or 0 end,
            setLocalVar = function(_, k, v) vars[k] = v end,
            getSpellListId = function() return 21 end,
            getMobMod = function() return 35 end,
            setSpellList = function(_, id) spellList = id end,
            setMobMod = function() end,
            addListener = function(_, event, name) listener = name end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(vars.providence == 1)
        assert(spellList == 506)
        assert(msg == xi.msg.basic.USES)
        assert(listener == 'PROVIDENCE_MAGIC_START')
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
    end)
end)
