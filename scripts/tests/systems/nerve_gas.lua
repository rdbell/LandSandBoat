require('scripts/actions/mobskills/nerve_gas')

describe('Nerve Gas mob skill', function()
    it('gates by pool/animation sub and applies Curse and Poison only after processing', function()
        local gas = require('scripts/actions/mobskills/nerve_gas')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local pool, animSub = 0, 0
        local mob = {
            getPool = function() return pool end,
            getAnimationSub = function() return animSub end,
            getMainLvl = function() return 75 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ELEMENTAL }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(effects, { ... }) end

        pool, animSub = 999, 1
        assert(gas.onMobSkillCheck(target, mob, skill) == 1)
        pool, animSub = 999, 0
        assert(gas.onMobSkillCheck(target, mob, skill) == 0)
        pool = xi.mobPool.TINNIN
        animSub = 1
        assert(gas.onMobSkillCheck(target, mob, skill) == 0)

        assert(gas.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ELEMENTAL)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        gas.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ELEMENTAL)
        assert(effects[1][3] == xi.effect.CURSE_I and effects[1][4] == 50 and effects[1][5] == 0 and effects[1][6] == 420)
        assert(effects[2][3] == xi.effect.POISON and effects[2][4] == 20 and effects[2][5] == 3 and effects[2][6] == 60)
    end)
end)
