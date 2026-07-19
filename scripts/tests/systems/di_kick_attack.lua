require('scripts/actions/mobskills/di_kick_attack')

describe('Di Kick Attack mob skill', function()
    it('uses its normal-hit physical plan and applies only processed damage', function()
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        local kickAttack = require('scripts/actions/mobskills/di_kick_attack')
        assert(kickAttack.onMobSkillCheck({}, {}, {}) == 0)
        assert(kickAttack.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.primaryMessage == xi.msg.basic.HIT_DMG)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(kickAttack.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
