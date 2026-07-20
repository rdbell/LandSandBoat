require('scripts/actions/mobskills/bone_crunch')
describe('Bone Crunch mob skill', function()
    it('uses physical plan and Plague when processed', function()
        local skill = require('scripts/actions/mobskills/bone_crunch')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(damage[1] == 70 and status[1] == xi.effect.PLAGUE and status[2] == 5 and status[4] == 60)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
