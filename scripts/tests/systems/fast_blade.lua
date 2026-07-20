describe('Fast Blade mob skill', function()
    it('announces Fast Blade and applies its two-hit Slashing plan only after processing', function()
        local fastBlade = require('scripts/actions/mobskills/fast_blade')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, ready, damage = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 77 end,
            messageBasic = function(_, ...) ready = { ... } end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(fastBlade.onMobSkillCheck(target, mob, {}) == 0)
        assert(ready[1] == xi.msg.basic.READIES_WS and ready[2] == 0 and ready[3] == xi.weaponskill.FAST_BLADE)
        assert(fastBlade.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(fastBlade.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
