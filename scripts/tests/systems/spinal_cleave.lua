require('scripts/actions/mobskills/spinal_cleave')
describe('Spinal Cleave mob skill', function()
    it('requires anim sub 1 and uses slashing plan with crit', function()
        local cleave = require('scripts/actions/mobskills/spinal_cleave')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(cleave.onMobSkillCheck(target, mob, {}) == 1)
        anim = 1; assert(cleave.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(cleave.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2 and params.canCrit and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        cleave.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
