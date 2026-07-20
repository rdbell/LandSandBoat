require('scripts/actions/mobskills/sweeping_flail')
describe('Sweeping Flail mob skill', function()
    it('requires behind target (55) and uses slashing plan', function()
        local skill = require('scripts/actions/mobskills/sweeping_flail')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, behind = nil, nil, false
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 55); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = false; assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = true; assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
