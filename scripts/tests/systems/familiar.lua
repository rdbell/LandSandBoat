require('scripts/actions/mobskills/familiar')

describe('Familiar mob skill', function()
    it('allows use, applies Familiar buffs to the current pet, and sets its message', function()
        local familiar = require('scripts/actions/mobskills/familiar')
        local apply = xi.pet.applyFamiliarBuffs
        local pet, call, message = {}, nil, nil
        local mob = { getPet = function() return pet end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.pet.applyFamiliarBuffs = function(owner, target) call = { owner, target } end
        assert(familiar.onMobSkillCheck({}, mob, skill) == 0)
        assert(familiar.onMobWeaponSkill(mob, {}, skill, {}) == 0)
        xi.pet.applyFamiliarBuffs = apply
        assert(call[1] == mob and call[2] == pet and message == xi.msg.basic.FAMILIAR_MOB)
    end)
end)
