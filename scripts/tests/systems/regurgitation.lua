require('scripts/actions/mobskills/regurgitation')
describe('Regurgitation mob skill', function()
    it('uses water magical plan and applies Bind without breaking Bind on damage', function()
        local skillObj = require('scripts/actions/mobskills/regurgitation')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, opts, statusParams = nil, nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, dmg, attacker, at, dt, o)
                damage = { dmg, attacker, at, dt }; opts = o
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skillObj.onMobSkillCheck(target, mob, {}) == 0 and skillObj.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.element == xi.element.WATER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillObj.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and opts and opts.breakBind == false)
        assert(statusParams[3] == xi.effect.BIND and statusParams[4] == 1 and statusParams[6] == 120)
    end)
end)
