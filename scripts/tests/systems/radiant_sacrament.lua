require('scripts/actions/mobskills/radiant_sacrament')
describe('Radiant Sacrament mob skill', function()
    it('uses its slashing physical plan and applies Magic Defense Down after processing', function()
        local sacrament = require('scripts/actions/mobskills/radiant_sacrament')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(sacrament.onMobSkillCheck(target, mob, {}) == 0 and sacrament.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        sacrament.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.MAGIC_DEF_DOWN and statusParams[4] == 20 and statusParams[6] == 60)
    end)
end)
