require('scripts/actions/mobskills/spirits_within')
describe('Spirits Within mob skill', function()
    it('emits READIES_WS for normal pools and uses TP-scaled breath plan', function()
        local within = require('scripts/actions/mobskills/spirits_within')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local tpf = xi.combat.physical.calculateTPfactor
        local params, damage, msg, showText, pool = nil, nil, nil, nil, 0
        local mob = {
            getPool = function() return pool end,
            messageBasic = function(_, mid, p0, p1) msg = { mid, p0, p1 } end,
            getMaxHP = function() return 5000 end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            showText = function(_, actor, textID) showText = { actor, textID } end,
        }
        local skill = { getTP = function() return 1000 end }
        assert(within.onMobSkillCheck(target, mob, skill) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == 39)
        pool = xi.mobPool.THRONE_ROOM_VOLKER
        msg = nil
        assert(within.onMobSkillCheck(target, mob, skill) == 0 and msg == nil)
        xi.combat.physical.calculateTPfactor = function(tp, tab)
            assert(tp == 1000 and tab[1] == 0.0625 and tab[2] == 0.1875 and tab[3] == 0.46875)
            return 0.0625
        end
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.BREATH, damageType = xi.damageType.ELEMENTAL }
        end
        xi.mobskills.processDamage = function() return false end
        pool = 0
        assert(within.onMobWeaponSkill(mob, target, skill, {}) == 100)
        assert(params.percentMultipier == 0.0625 and params.damageCap == 5000 and damage == nil and showText == nil)
        xi.mobskills.processDamage = function() return true end
        pool = xi.mobPool.THRONE_ROOM_VOLKER
        showText = nil
        within.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        xi.combat.physical.calculateTPfactor = tpf
        assert(damage[1] == 100 and showText and showText[1] == mob)
    end)
end)
