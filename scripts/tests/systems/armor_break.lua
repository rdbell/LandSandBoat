require('scripts/actions/mobskills/armor_break')

describe('Armor Break mob skill', function()
    it('uses its Slashing physical plan and applies TP-scaled Defense Down only after processing', function()
        local params, damage, defenseDown = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) defenseDown = { ... } end

        local armorBreak = require('scripts/actions/mobskills/armor_break')
        assert(armorBreak.onMobSkillCheck({}, {}, {}) == 0)
        assert(armorBreak.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and defenseDown == nil)

        xi.mobskills.processDamage = function() return true end
        assert(armorBreak.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(defenseDown[3] == xi.effect.DEFENSE_DOWN and defenseDown[4] == 25 and defenseDown[5] == 0 and defenseDown[6] == 210)
    end)
end)
