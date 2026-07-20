require('scripts/actions/mobskills/severing_fang')
describe('Severing Fang mob skill', function()
    it('rejects behind targets and applies Defense Down after processing', function()
        local fang = require('scripts/actions/mobskills/severing_fang')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, true
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            isBehind = function() return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(fang.onMobSkillCheck(target, mob, {}) == 1)
        behind = false; assert(fang.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(fang.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2.5 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        fang.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 80 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 50 and statusParams[6] == 90)
    end)
end)
