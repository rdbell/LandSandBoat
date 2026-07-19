require('scripts/actions/mobskills/death_blossom')

describe('Death Blossom mob skill', function()
    it('uses its three-hit physical plan and applies Magic Evasion Down only after damage processing', function()
        local params, damage, magicEvasionDown = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) magicEvasionDown = { ... } end

        local blossom = require('scripts/actions/mobskills/death_blossom')
        assert(blossom.onMobSkillCheck({}, {}, {}) == 0)
        assert(blossom.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3)
        assert(params.fTP[1] == 1.125 and params.fTP[2] == 1.125 and params.fTP[3] == 1.125)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and magicEvasionDown == nil)

        xi.mobskills.processDamage = function() return true end
        assert(blossom.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(magicEvasionDown[3] == xi.effect.MAGIC_EVASION_DOWN and magicEvasionDown[4] == 10 and magicEvasionDown[5] == 0 and magicEvasionDown[6] == 60)
    end)
end)
