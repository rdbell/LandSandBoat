require('scripts/actions/mobskills/claw_storm')
describe('Claw Storm mob skill', function()
    it('uses three-hit plan and Poison when processed', function()
        local skill = require('scripts/actions/mobskills/claw_storm')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 3 and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(damage == 90 and status[1] == xi.effect.POISON and status[2] == 7 and status[3] == 3)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
