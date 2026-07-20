require('scripts/actions/mobskills/stringing_pummel')
describe('Stringing Pummel mob skill', function()
    it('uses sixfold H2H plan with TP-scaled crit chance', function()
        local pummel = require('scripts/actions/mobskills/stringing_pummel')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(pummel.onMobSkillCheck(target, mob, {}) == 0 and pummel.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 6 and params.fTP[1] == 0.75 and params.canCrit and params.criticalChance[3] == 0.45 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        pummel.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 90)
    end)
end)
