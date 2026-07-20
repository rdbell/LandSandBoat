require('scripts/actions/mobskills/metatron_torment')

describe('Metatron Torment mob skill', function()
    it('uses its Slashing physical plan and applies Defense Down only after processing', function()
        local params, damage, defenseDown = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) defenseDown = { ... } end

        local metatron = require('scripts/actions/mobskills/metatron_torment')
        assert(metatron.onMobSkillCheck({}, {}, {}) == 0)
        assert(metatron.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.75 and params.fTP[2] == 2.75 and params.fTP[3] == 2.75)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and defenseDown == nil)

        xi.mobskills.processDamage = function() return true end
        assert(metatron.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(defenseDown[3] == xi.effect.DEFENSE_DOWN and defenseDown[4] == 19 and defenseDown[5] == 0 and defenseDown[6] == 120)
    end)
end)
