require('scripts/actions/mobskills/ore_toss_ranged')
describe('Ore Toss Ranged mob skill', function()
    it('uses ranged blunt plan with distance damage scaling', function()
        local skill = require('scripts/actions/mobskills/ore_toss_ranged')
        local params, dmg = nil, nil
        local origR, origD = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function(mob, target, sk, action, info)
            dmg = info.damage
            return true
        end
        local mob = { getWeaponDmg = function() return 40 end, checkDistance = function() return 1 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.0 and params.attackType == xi.attackType.RANGED and params.skipParry)
        assert(dmg == 100)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = origR, origD
    end)
end)
