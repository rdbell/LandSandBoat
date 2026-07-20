require('scripts/actions/mobskills/bonebreaking_barrage')
describe('Bonebreaking Barrage mob skill', function()
    it('uses physical plan and Max HP Down plus Weight when processed', function()
        local skill = require('scripts/actions/mobskills/bonebreaking_barrage')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getWeaponDmg = function() return 55 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            statuses[#statuses + 1] = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.fTP[1] == 2.0 and #statuses == 0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(damage[1] == 90)
        assert(statuses[1][1] == xi.effect.MAX_HP_DOWN and statuses[1][2] == 50 and statuses[1][4] == 60)
        assert(statuses[2][1] == xi.effect.WEIGHT and statuses[2][2] == 50 and statuses[2][4] == 30)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
