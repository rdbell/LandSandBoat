require('scripts/actions/mobskills/shield_bash_1')
describe('Shield Bash 1 mob skill', function()
    it('sets WEAPONSKILL_FINISH category and applies Stun after processing', function()
        local bash = require('scripts/actions/mobskills/shield_bash_1')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, category = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local action = { setCategory = function(_, c) category = c end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 55, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(bash.onMobSkillCheck(target, mob, {}) == 0 and bash.onMobWeaponSkill(mob, target, {}, action) == 55)
        assert(category == xi.action.category.WEAPONSKILL_FINISH and params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        bash.onMobWeaponSkill(mob, target, {}, action)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 55 and statusParams[3] == xi.effect.STUN and statusParams[6] == 7)
    end)
end)
