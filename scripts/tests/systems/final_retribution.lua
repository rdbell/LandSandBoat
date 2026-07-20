describe('Final Retribution mob skill', function()
    it('uses its slashing plan and stuns only after processing damage', function()
        local finalRetribution = require('scripts/actions/mobskills/final_retribution')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) stun = { ... } end
        assert(finalRetribution.onMobSkillCheck(target, mob, {}) == 0)
        assert(finalRetribution.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3.2 and params.fTP[2] == 3.2 and params.fTP[3] == 3.2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and stun == nil)
        xi.mobskills.processDamage = function() return true end
        assert(finalRetribution.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(stun[1] == xi.effect.STUN and stun[2] == 1 and stun[3] == 0 and stun[4] == 4)
    end)
end)
