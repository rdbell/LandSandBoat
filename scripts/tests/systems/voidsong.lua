require('scripts/actions/mobskills/voidsong')

describe('Voidsong mob skill', function()
    it('rejects a silenced mob and erases caster plus target effects when usable', function()
        local voidsong = require('scripts/actions/mobskills/voidsong')
        local casterErased, message = false, nil
        local mob = {
            hasStatusEffect = function(_, effect) return effect == xi.effect.SILENCE end,
            eraseAllStatusEffect = function() casterErased = true end,
        }
        local target = {
            dispelAllStatusEffect = function() return 2 end,
            eraseAllStatusEffect = function() return 3 end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(voidsong.onMobSkillCheck(target, mob, skill) == 1)
        mob.hasStatusEffect = function() return false end
        assert(voidsong.onMobSkillCheck(target, mob, skill) == 0)
        assert(voidsong.onMobWeaponSkill(mob, target, skill, {}) == 5)
        assert(casterErased and message == xi.msg.basic.DISAPPEAR_NUM)
    end)

    it('reports no effect when neither target erase removes anything', function()
        local voidsong = require('scripts/actions/mobskills/voidsong')
        local message = nil
        local mob = { eraseAllStatusEffect = function() end }
        local target = { dispelAllStatusEffect = function() return 0 end, eraseAllStatusEffect = function() return 0 end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(voidsong.onMobWeaponSkill(mob, target, skill, {}) == 0)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
