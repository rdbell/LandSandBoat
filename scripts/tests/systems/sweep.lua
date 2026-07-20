require('scripts/actions/mobskills/sweep')
describe('Sweep mob skill', function()
    it('uses slashing three-shadow plan and stun after processing', function()
        local skill = require('scripts/actions/mobskills/sweep')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
