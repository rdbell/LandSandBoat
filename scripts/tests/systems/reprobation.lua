require('scripts/actions/mobskills/reprobation')
describe('Reprobation mob skill', function()
    it('dispels all DISPELABLE|FOOD effects and reports count', function()
        local repro = require('scripts/actions/mobskills/reprobation')
        local flags, count, message = nil, 0, nil
        local target = {
            dispelAllStatusEffect = function(_, f)
                flags = f
                return count
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(repro.onMobSkillCheck(target, {}, skill) == 0)
        count = 0
        assert(repro.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        count = 3
        assert(repro.onMobWeaponSkill({}, target, skill, {}) == 3)
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
