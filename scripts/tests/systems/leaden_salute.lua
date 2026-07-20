require('scripts/actions/mobskills/leaden_salute')

describe('Leaden Salute mob skill', function()
    it('uses its Dark magical dSTAT plan and damages only after processing succeeds', function()
        local leadenSalute = require('scripts/actions/mobskills/leaden_salute')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        assert(leadenSalute.onMobSkillCheck(target, mob, {}) == 0)
        assert(leadenSalute.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4.25 and params.fTP[3] == 4.75)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.dStatMultiplier == 2 and params.dStatAttackerMod == xi.mod.AGI and params.dStatDefenderMod == xi.mod.INT)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(leadenSalute.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
