require('scripts/actions/mobskills/stupor_spores')
describe('Stupor Spores mob skill', function()
    it('uses none-element magical plan and applies Sleep after processing', function()
        local spores = require('scripts/actions/mobskills/stupor_spores')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        math.random = function(a, b) assert(a == 15 and b == 60); return 30 end
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ELEMENTAL }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spores.onMobSkillCheck(target, mob, {}) == 0 and spores.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.baseDamage == 42 and params.fTP[1] == 1 and params.dStatMultiplier == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        spores.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 50 and statusParams[3] == xi.effect.SLEEP_I and statusParams[6] == 30)
    end)
end)
