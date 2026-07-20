require('scripts/actions/mobskills/potent_lunge')
describe('Potent Lunge mob skill', function()
    it('requires animation sub 0 and uses slashing physical plan', function()
        local lunge = require('scripts/actions/mobskills/potent_lunge')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local animSub, params, damage = 0, nil, nil
        local mob = { getAnimationSub = function() return animSub end, getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 1; assert(lunge.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 0; assert(lunge.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(lunge.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 1.5 and params.damageType == xi.damageType.SLASHING and damage == nil)
        xi.mobskills.processDamage = function() return true end
        lunge.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
