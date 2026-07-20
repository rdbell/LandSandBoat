require('scripts/actions/mobskills/sleet_blast')
describe('Sleet Blast mob skill', function()
    it('requires anim sub 1 and uses magical plan', function()
        local skillmod = require('scripts/actions/mobskills/sleet_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = {
            getAnimationSub = function() return anim end,
            getMainLvl = function() return 70 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 1)
        anim = 1; assert(skillmod.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.baseDamage == 72 and params.element == xi.element.ICE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
