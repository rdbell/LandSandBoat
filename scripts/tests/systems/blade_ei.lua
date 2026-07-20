require('scripts/actions/mobskills/blade_ei')
describe('Blade Ei mob skill', function()
    it('uses Dark magical plan with INT dStat and TP-scaled fTP', function()
        local skill = require('scripts/actions/mobskills/blade_ei')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[2] == 1.5 and params.fTP[3] == 2.0)
        assert(params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 100)
    end)
end)
