require('scripts/actions/mobskills/savage_blade')
describe('Savage Blade mob skill', function()
    it('emits READIES_WS for normal pools and uses two-hit slashing plan', function()
        local blade = require('scripts/actions/mobskills/savage_blade')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, msg, showText = nil, nil, nil, nil
        local pool = 0
        local mob = {
            getPool = function() return pool end,
            messageBasic = function(_, mid, p0, p1) msg = { mid, p0, p1 } end,
            getWeaponDmg = function() return 55 end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            showText = function(_, actor, textID) showText = { actor, textID } end,
        }
        assert(blade.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 42)
        pool = xi.mobPool.QUBIA_ARENA_TRION
        msg = nil
        assert(blade.onMobSkillCheck(target, mob, {}) == 0 and msg == nil)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        pool = 0
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.numHits == 2 and params.fTP[1] == 1.0 and params.fTP[2] == 1.75 and params.fTP[3] == 3.5)
        assert(damage == nil and showText == nil)
        xi.mobskills.processDamage = function() return true end
        blade.onMobWeaponSkill(mob, target, {}, {})
        assert(damage[1] == 200 and showText == nil)
        pool = xi.mobPool.QUBIA_ARENA_TRION
        showText = nil
        blade.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(showText and showText[1] == mob)
    end)
end)
