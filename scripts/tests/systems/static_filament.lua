require('scripts/actions/mobskills/static_filament')
describe('Static Filament mob skill', function()
    it('uses thunder magical plan and applies Stun after processing', function()
        local fil = require('scripts/actions/mobskills/static_filament')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(fil.onMobSkillCheck(target, mob, {}) == 0 and fil.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[2] == 1.5 and params.fTP[3] == 2.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        fil.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 90 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
