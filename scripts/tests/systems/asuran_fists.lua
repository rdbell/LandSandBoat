require('scripts/actions/mobskills/asuran_fists')
describe('Asuran Fists mob skill', function()
    it('gates level 70 and uses eight-hit H2H plan', function()
        local skill = require('scripts/actions/mobskills/asuran_fists')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {
            getMainLvl = function() return 69 end,
            getWeaponDmg = function() return 45 end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getMainLvl = function() return 70 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.numHits == 8 and params.accuracyModifier[2] == 30 and params.accuracyModifier[3] == 60)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_8 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 150)
    end)
end)
