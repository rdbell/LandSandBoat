require('scripts/actions/mobskills/weapon_bash')
describe('Weapon Bash mob skill', function()
    it('sets WEAPONSKILL_FINISH and applies stun after processing', function()
        local skill = require('scripts/actions/mobskills/weapon_bash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, category = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local action = { setCategory = function(_, c) category = c end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, action) == 100)
        assert(category == xi.action.category.WEAPONSKILL_FINISH and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, action)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STUN and statusParams[6] == 7)
    end)
end)
