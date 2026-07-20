describe('Feral Peck mob skill', function()
    it('uses target HP and its bypassing Piercing plan, then damages only after processing', function()
        local feralPeck = require('scripts/actions/mobskills/feral_peck')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = {
            getHP = function() return 777 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(feralPeck.onMobSkillCheck(target, mob, {}) == 0)
        assert(feralPeck.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 777 and params.numHits == 1 and params.fTP[1] == 0.9 and params.fTP[2] == 0.9 and params.fTP[3] == 0.9)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipFSTR and params.skipPDIF and params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(feralPeck.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
