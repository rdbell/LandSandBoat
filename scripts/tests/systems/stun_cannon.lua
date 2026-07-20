require('scripts/actions/mobskills/stun_cannon')
describe('Stun Cannon mob skill', function()
    it('rejects behind targets and applies Paralysis after processing', function()
        local cannon = require('scripts/actions/mobskills/stun_cannon')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, true
        local mob = { getMainLvl = function() return 60 end }
        local target = {
            isBehind = function() return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(cannon.onMobSkillCheck(target, mob, {}) == 1)
        behind = false; assert(cannon.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(cannon.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.baseDamage == 62 and params.fTP[1] == 4.50 and params.element == xi.element.THUNDER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        cannon.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 200 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 20 and statusParams[6] == 120)
    end)
end)
