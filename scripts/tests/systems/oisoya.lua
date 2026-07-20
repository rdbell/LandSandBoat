require('scripts/actions/mobskills/oisoya')
describe('Oisoya mob skill', function()
    it('admits bow mode with ShouldOisoya and uses ranged physical plan', function()
        local skill = require('scripts/actions/mobskills/oisoya')
        local params = nil
        local origR, origD = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 150, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local vars = { ['[Tenzen]ShouldOisoya'] = 1 }
        local target = { takeDamage = function() end }
        local mob = {
            getAnimationSub = function() return 5 end,
            getLocalVar = function(_, k) return vars[k] or 0 end,
            getWeaponDmg = function() return 40 end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        vars['[Tenzen]ShouldOisoya'] = 0
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        vars['[Tenzen]ShouldOisoya'] = 1
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.fTP[1] == 5.5 and params.attackMultiplier[1] == 2.75 and params.skipParry)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = origR, origD
    end)
end)
