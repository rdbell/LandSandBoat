require('scripts/actions/mobskills/violent_rupture')
describe('Violent Rupture mob skill', function()
    it('uses fire breath plan and STR down after processing', function()
        local skill = require('scripts/actions/mobskills/violent_rupture')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = {}
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.percentMultipier == 0.10 and params.damageCap == 200 and params.element == xi.element.FIRE and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STR_DOWN and statusParams[4] == 50 and statusParams[5] == 3 and statusParams[6] == 120)
    end)
end)
