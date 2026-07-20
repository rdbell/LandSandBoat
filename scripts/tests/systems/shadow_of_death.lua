require('scripts/actions/mobskills/shadow_of_death')
describe('Shadow of Death mob skill', function()
    it('uses dark magical plan with dStat and damages only after processing', function()
        local death = require('scripts/actions/mobskills/shadow_of_death')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(death.onMobSkillCheck(target, mob, {}) == 0 and death.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.baseDamage == 52 and params.fTP[2] == 2.5 and params.dStatMultiplier == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        death.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 120)
    end)
end)
