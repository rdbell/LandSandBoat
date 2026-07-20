require('scripts/actions/mobskills/trueflight')
describe('Trueflight mob skill', function()
    it('uses light magical plan with AGI/INT dStat', function()
        local skill = require('scripts/actions/mobskills/trueflight')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 4.0 and params.fTP[2] == 4.25 and params.fTP[3] == 4.75)
        assert(params.dStatMultiplier == 2 and params.dStatAttackerMod == xi.mod.AGI and params.dStatDefenderMod == xi.mod.INT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
