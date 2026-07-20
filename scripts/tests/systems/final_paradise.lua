describe('Final Paradise mob skill', function()
    it('uses its TP-scaled Hand-to-Hand plan and reports paradise only after processing', function()
        local finalParadise = require('scripts/actions/mobskills/final_paradise')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, message = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(finalParadise.onMobSkillCheck(target, mob, skill) == 0)
        assert(finalParadise.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(finalParadise.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
        assert(message == xi.msg.basic.MOTE_OF_PARADISE)
    end)
end)
