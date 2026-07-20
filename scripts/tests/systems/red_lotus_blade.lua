require('scripts/actions/mobskills/red_lotus_blade')

describe('Red Lotus Blade mob skill', function()
    it('emits READIES_WS for normal pools and uses fire magical plan', function()
        local blade = require('scripts/actions/mobskills/red_lotus_blade')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, msg, showText = nil, nil, nil, nil
        local pool = 0
        local mob = {
            getPool = function() return pool end,
            messageBasic = function(_, mid, p0, p1) msg = { mid, p0, p1 } end,
            getMainLvl = function() return 50 end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            showText = function(_, actor, textID) showText = { actor, textID } end,
        }
        assert(blade.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 34)
        pool = xi.mobPool.QUBIA_ARENA_TRION
        msg = nil
        assert(blade.onMobSkillCheck(target, mob, {}) == 0 and msg == nil)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        pool = 0
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.fTP[2] == 2.38 and params.fTP[3] == 3)
        assert(params.element == xi.element.FIRE and damage == nil and showText == nil)
        xi.mobskills.processDamage = function() return true end
        blade.onMobWeaponSkill(mob, target, {}, {})
        assert(damage[1] == 123 and showText == nil)
        pool = xi.mobPool.QUBIA_ARENA_TRION
        showText = nil
        blade.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(showText and showText[1] == mob)
    end)
end)
