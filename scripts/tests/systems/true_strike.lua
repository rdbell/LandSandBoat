require('scripts/actions/mobskills/true_strike')
describe('True Strike mob skill', function()
    it('readies WS 166 and uses guaranteed-crit blunt plan', function()
        local skill = require('scripts/actions/mobskills/true_strike')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, ready = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 80 end,
            messageBasic = function(_, msg, p0, p1) ready = { msg, p0, p1 } end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(ready[1] == xi.msg.basic.READIES_WS and ready[2] == 0 and ready[3] == 166)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.canCrit and params.criticalChance[1] == 1.0 and params.accuracyModifier[1] == -50 and params.attackMultiplier[1] == 2.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
