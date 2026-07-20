require('scripts/actions/mobskills/maru_kala')

describe('Maru Kala mob skill', function()
    it('uses its two-hit hand-to-hand plan and damages only after processing', function()
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        local skillObject = require('scripts/actions/mobskills/maru_kala')
        assert(skillObject.onMobSkillCheck(target, mob, {}) == 0 and skillObject.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2 and params.fTP[1] == 3.125 and params.fTP[2] == 7.25 and params.fTP[3] == 11.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillObject.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
    end)
end)
