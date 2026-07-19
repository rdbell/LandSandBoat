require('scripts/actions/mobskills/demoralizing_roar')

describe('Demoralizing Roar mob skill', function()
    it('requires the target beside the mob and requests TP-scaled Attack Down', function()
        local beside = false
        local target = { isBeside = function() return beside end }
        local request = nil
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(...) request = { ... } end
        local skill = { getTP = function() return 1500 end }

        local roar = require('scripts/actions/mobskills/demoralizing_roar')
        assert(roar.onMobSkillCheck(target, {}, {}) == 1)
        beside = true
        assert(roar.onMobSkillCheck(target, {}, {}) == 0)
        roar.onMobWeaponSkill({}, target, skill, {})
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(request[3] == xi.effect.ATTACK_DOWN and request[4] == 500 and request[5] == 0 and request[6] == 60)
    end)
end)
