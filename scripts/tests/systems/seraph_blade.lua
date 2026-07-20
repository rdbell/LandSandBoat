require('scripts/actions/mobskills/seraph_blade')
describe('Seraph Blade mob skill', function()
    it('emits READIES_WS and uses light magical plan', function()
        local blade = require('scripts/actions/mobskills/seraph_blade')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, msg = nil, nil, nil
        local mob = {
            messageBasic = function(_, mid, p0, p1) msg = { mid, p0, p1 } end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(blade.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 37)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 111, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 111)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[2] == 2.5 and params.fTP[3] == 3.0)
        assert(params.element == xi.element.LIGHT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        blade.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 111)
    end)
end)
