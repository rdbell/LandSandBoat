require('scripts/actions/mobskills/sickle_slash')
describe('Sickle Slash mob skill', function()
    it('gates Ghrah anim sub and uses blunt physical plan with crit', function()
        local slash = require('scripts/actions/mobskills/sickle_slash')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, family, anim = nil, nil, 0, 0
        local mob = {
            getFamily = function() return family end,
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        family = xi.mobFamily.GHRAH; anim = 0
        assert(slash.onMobSkillCheck(target, mob, {}) == 1)
        anim = 2; assert(slash.onMobSkillCheck(target, mob, {}) == 0)
        family = 0; anim = 0; assert(slash.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(slash.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2 and params.attackMultiplier[1] == 1.5 and params.canCrit and damage == nil)
        xi.mobskills.processDamage = function() return true end
        slash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
