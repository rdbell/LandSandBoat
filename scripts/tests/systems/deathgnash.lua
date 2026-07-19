require('scripts/actions/mobskills/deathgnash')

describe('Deathgnash mob skill', function()
    it('uses target HP to leave one HP and resets enmity only after processed damage', function()
        local params, damage, resetTarget = nil, nil, nil
        local mob = { resetEnmity = function(_, target) resetTarget = target end }
        local target = {
            getHP = function() return 500 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        local deathgnash = require('scripts/actions/mobskills/deathgnash')
        assert(deathgnash.onMobSkillCheck({}, {}, {}) == 0)
        assert(deathgnash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 499 and params.numHits == 1)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipFSTR and params.skipPDIF and params.skipParry and params.skipGuard and params.skipBlock)
        assert(damage == nil and resetTarget == nil)

        xi.mobskills.processDamage = function() return true end
        assert(deathgnash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(resetTarget == target)
    end)
end)
