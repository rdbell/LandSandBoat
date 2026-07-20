require('scripts/actions/mobskills/skullbreaker')
describe('Skullbreaker mob skill', function()
    it('emits READIES_WS and applies INT Down after processing', function()
        local brk = require('scripts/actions/mobskills/skullbreaker')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, msg = nil, nil, nil, nil
        local mob = {
            messageBasic = function(_, mid, p0, p1) msg = { mid, p0, p1 } end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(brk.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 165)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(brk.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        brk.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 50 and statusParams[3] == xi.effect.INT_DOWN and statusParams[4] == 10 and statusParams[5] == 9 and statusParams[6] == 120)
    end)
end)
