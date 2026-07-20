require('scripts/actions/mobskills/catapult')
describe('Catapult mob skill', function()
    it('admits out-of-range targets and uses ranged plan', function()
        local skill = require('scripts/actions/mobskills/catapult')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local dist = 1
        local mob = {
            getWeaponDmg = function() return 50 end,
            checkDistance = function() return dist end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        dist = 3
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.attackType == xi.attackType.RANGED and params.fTP[1] == 3.0 and params.skipParry)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage[1] == 100)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
    end)
end)
