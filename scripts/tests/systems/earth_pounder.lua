require('scripts/actions/mobskills/earth_pounder')

describe('Earth Pounder mob skill', function()
    it('uses its Earth plan, battlefield fTP override, and processed DEX Down', function()
        local pounder = require('scripts/actions/mobskills/earth_pounder')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            getPool = function() return 0 end,
            getBattlefield = function() return nil end,
        }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end

        assert(pounder.onMobSkillCheck(target, mob, skill) == 0)
        assert(pounder.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and effect == nil)

        mob.getPool = function() return xi.mobPool.PLATOON_SCORPION end
        assert(pounder.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)

        mob.getBattlefield = function() return { getLocalVar = function(_, name) assert(name == 'scorpionsDefeated'); return 3 end } end
        xi.mobskills.processDamage = function() return true end
        assert(pounder.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove

        assert(params.fTP[1] == 3.5 and params.fTP[2] == 3.5 and params.fTP[3] == 3.5)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
        assert(effect[1] == xi.effect.DEX_DOWN and effect[2] == 10 and effect[3] == 9 and effect[4] == 180)
    end)
end)
