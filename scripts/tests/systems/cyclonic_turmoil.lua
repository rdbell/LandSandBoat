require('scripts/actions/mobskills/cyclonic_turmoil')

describe('Cyclonic Turmoil mob skill', function()
    it('uses Wind magical parameters and dispels only after processing succeeds', function()
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, dispel = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end,
            dispelAllStatusEffect = function(_, flag) dispel = flag end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p)
            params = p
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end

        local skill = require('scripts/actions/mobskills/cyclonic_turmoil')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and dispel == nil)

        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
        assert(dispel == xi.effectFlag.DISPELABLE)
    end)
end)
