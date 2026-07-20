require('scripts/actions/mobskills/trebuchet')
describe('Trebuchet mob skill', function()
    it('uses ranged blunt plan with skip parry/guard/block', function()
        local skill = require('scripts/actions/mobskills/trebuchet')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.RANGED, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.6 and params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
