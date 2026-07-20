require('scripts/actions/mobskills/silencing_microtube')
describe('Silencing Microtube mob skill', function()
    it('uses none-element magical plan and applies Silence after processing', function()
        local tube = require('scripts/actions/mobskills/silencing_microtube')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(tube.onMobSkillCheck(target, mob, {}) == 0 and tube.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.baseDamage == 30 and params.fTP[1] == 12.25 and params.element == xi.element.NONE and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        tube.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 90 and statusParams[3] == xi.effect.SILENCE and statusParams[6] == 60)
    end)
end)
