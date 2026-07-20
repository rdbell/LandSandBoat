describe('Feather Maelstrom mob skill', function()
    it('uses its Piercing plan and applies Bio and Amnesia only after processing', function()
        local featherMaelstrom = require('scripts/actions/mobskills/feather_maelstrom')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) table.insert(effects, { ... }) end
        assert(featherMaelstrom.onMobSkillCheck(target, mob, {}) == 0)
        assert(featherMaelstrom.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.8 and params.fTP[2] == 2.8 and params.fTP[3] == 2.8)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(featherMaelstrom.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.BIO and effects[1][2] == 6 and effects[1][3] == 3 and effects[1][4] == 60 and effects[1][5] == 0 and effects[1][6] == 15)
        assert(effects[2][1] == xi.effect.AMNESIA and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 60)
    end)
end)
