require('scripts/actions/mobskills/deal_out')

describe('Deal Out mob skill', function()
    it('uses its physical plan and resets enmity only for processed notorious damage', function()
        local params, damage, resetTarget = nil, nil, nil
        local notorious = false
        local mob = {
            getWeaponDmg = function() return 77 end,
            isMobType = function(_, type) return notorious and type == xi.mobType.NOTORIOUS end,
            resetEnmity = function(_, target) resetTarget = target end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        local dealOut = require('scripts/actions/mobskills/deal_out')
        assert(dealOut.onMobSkillCheck({}, {}, {}) == 0)
        assert(dealOut.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and resetTarget == nil)

        xi.mobskills.processDamage = function() return true end
        assert(dealOut.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(resetTarget == nil)

        notorious = true
        assert(dealOut.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(resetTarget == target)
    end)
end)
