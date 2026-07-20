require('scripts/actions/mobskills/lava_spit')

describe('Lava Spit mob skill', function()
    it('rejects targets behind the caster', function()
        local lavaSpit = require('scripts/actions/mobskills/lava_spit')
        local mob = {}
        local target = { isBehind = function(_, source, angle) assert(source == mob and angle == 48); return true end }
        assert(lavaSpit.onMobSkillCheck(target, mob, {}) == 1)
        target.isBehind = function() return false end
        assert(lavaSpit.onMobSkillCheck(target, mob, {}) == 0)
    end)

    it('uses a Fire magical plan and damages only after processing succeeds', function()
        local lavaSpit = require('scripts/actions/mobskills/lava_spit')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(lavaSpit.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(lavaSpit.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
