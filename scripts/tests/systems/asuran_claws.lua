require('scripts/actions/mobskills/asuran_claws')
describe('Asuran Claws mob skill', function()
    it('denies all-fours and uses six-hit plan', function()
        local skill = require('scripts/actions/mobskills/asuran_claws')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {
            getAnimationSub = function() return 0 end,
            getWeaponDmg = function() return 50 end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.numHits == 6 and params.fTP[1] == 0.8 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_6)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 120)
    end)
end)
