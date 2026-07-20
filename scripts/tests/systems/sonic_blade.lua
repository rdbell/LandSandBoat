require('scripts/actions/mobskills/sonic_blade')
describe('Sonic Blade mob skill', function()
    it('requires sword form and uses slashing physical plan', function()
        local blade = require('scripts/actions/mobskills/sonic_blade')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(blade.onMobSkillCheck(target, mob, {}) == 1)
        anim = 1; assert(blade.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.fTP[1] == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        blade.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 70)
    end)
end)
