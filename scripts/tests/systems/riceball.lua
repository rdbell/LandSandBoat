require('scripts/actions/mobskills/riceball')
describe('Riceball mob skill', function()
    it('applies fixed combat mods and emits NONE', function()
        local rice = require('scripts/actions/mobskills/riceball')
        local adds, sets, message = {}, {}, nil
        local mob = {
            addMod = function(_, mod, value) adds[#adds+1] = { mod, value } end,
            setMod = function(_, mod, value) sets[#sets+1] = { mod, value } end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(rice.onMobSkillCheck({}, mob, skill) == 0)
        assert(rice.onMobWeaponSkill(mob, {}, skill, {}) == 0)
        assert(adds[1][1] == xi.mod.ATT and adds[1][2] == 50)
        assert(adds[2][1] == xi.mod.DEF and adds[2][2] == 30)
        assert(sets[1][1] == xi.mod.DOUBLE_ATTACK and sets[1][2] == 5)
        assert(sets[2][1] == xi.mod.DMGMAGIC and sets[2][2] == -2500)
        assert(message == xi.msg.basic.NONE)
    end)
end)
