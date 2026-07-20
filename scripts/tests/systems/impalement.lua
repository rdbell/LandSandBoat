describe('Impalement mob skill', function()
    it('uses target HP in its bypassing physical plan and resets enmity only after processing', function()
        local impalement = require('scripts/actions/mobskills/impalement')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local params, damage, resets = nil, nil, 0
        local mob = { resetEnmity = function() resets = resets + 1 end }
        local target = {
            getHP = function() return 500 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 475, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(impalement.onMobSkillCheck(target, mob, {}) == 0)
        assert(impalement.onMobWeaponSkill(mob, target, {}, {}) == 475)
        assert(params.baseDamage == 500 and params.numHits == 1 and params.fTP[1] == .95 and params.fTP[2] == .95 and params.fTP[3] == .95)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipFSTR and params.skipPDIF and params.skipParry and params.skipGuard and params.skipBlock)
        assert(damage == nil and resets == 0)

        xi.mobskills.processDamage = function() return true end
        impalement.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process

        assert(damage[1] == 475 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(resets == 1)
    end)
end)
