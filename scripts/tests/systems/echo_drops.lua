require('scripts/actions/mobskills/echo_drops')

describe('Echo Drops mob skill', function()
    it('removes Silence when present', function()
        local drops = require('scripts/actions/mobskills/echo_drops')
        local removed, message = nil, nil
        local target = {
            hasStatusEffect = function(_, effect) return effect == xi.effect.SILENCE end,
            delStatusEffect = function(_, effect) removed = effect end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(drops.onMobSkillCheck(target, {}, skill) == 0)
        assert(drops.onMobWeaponSkill({}, target, skill, {}) == xi.effect.SILENCE)
        assert(removed == xi.effect.SILENCE and message == nil)
    end)

    it('reports magic no effect when Silence is absent', function()
        local drops = require('scripts/actions/mobskills/echo_drops')
        local message = nil
        local target = { hasStatusEffect = function() return false end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(drops.onMobWeaponSkill({}, target, skill, {}) == xi.effect.NONE)
        assert(message == xi.msg.basic.MAGIC_NO_EFFECT)
    end)
end)
