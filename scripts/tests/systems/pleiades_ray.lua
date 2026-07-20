require('scripts/actions/mobskills/pleiades_ray')

describe('Pleiades Ray mob skill', function()
    it('allows use at HPP <= 20 and applies seven statuses after processing', function()
        local ray = require('scripts/actions/mobskills/pleiades_ray')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local hpp, params, damage, calls = 20, nil, nil, {}
        local mob = { getHPP = function() return hpp end, getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        hpp = 21
        assert(ray.onMobSkillCheck(target, mob, {}) == 1)
        hpp = 20
        assert(ray.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ray.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 7 and params.element == xi.element.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        ray.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and #calls == 7)
        assert(calls[1][3] == xi.effect.PARALYSIS and calls[7][3] == xi.effect.SLOW and calls[7][4] == 1250)
    end)
end)
