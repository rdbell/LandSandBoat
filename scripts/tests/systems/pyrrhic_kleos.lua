require('scripts/actions/mobskills/pyrrhic_kleos')
describe('Pyrrhic Kleos mob skill', function()
    it('uses its fourfold piercing plan and applies TP-scaled Evasion Down after processing', function()
        local kleos = require('scripts/actions/mobskills/pyrrhic_kleos')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(kleos.onMobSkillCheck(target, mob, skill) == 0 and kleos.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.numHits == 4 and params.fTP[1] == 1.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        kleos.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(statusParams[3] == xi.effect.EVASION_DOWN and statusParams[4] == 10 and statusParams[6] == 60)
    end)
end)
