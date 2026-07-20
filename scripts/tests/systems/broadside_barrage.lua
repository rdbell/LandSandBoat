require('scripts/actions/mobskills/broadside_barrage')
describe('Broadside Barrage mob skill', function()
    it('uses physical crit plan and STR/VIT down when processed', function()
        local skill = require('scripts/actions/mobskills/broadside_barrage')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getWeaponDmg = function() return 50 end, getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            statuses[#statuses + 1] = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.canCrit and params.fTP[1] == 2.0 and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage[1] == 100)
        assert(statuses[1][1] == xi.effect.STR_DOWN and statuses[1][2] == 13 and statuses[1][4] == 120)
        assert(statuses[2][1] == xi.effect.VIT_DOWN and statuses[2][2] == 13)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
