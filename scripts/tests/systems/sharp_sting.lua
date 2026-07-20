require('scripts/actions/mobskills/sharp_sting')
describe('Sharp Sting mob skill', function()
    it('uses ranged move with physical attackType and skip flags', function()
        local sting = require('scripts/actions/mobskills/sharp_sting')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 18 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 27, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(sting.onMobSkillCheck(target, mob, {}) == 0 and sting.onMobWeaponSkill(mob, target, {}, {}) == 27)
        assert(params.fTP[1] == 1.5 and params.attackType == xi.attackType.PHYSICAL)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        sting.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 27)
    end)
end)
