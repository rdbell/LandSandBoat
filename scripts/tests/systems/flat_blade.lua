describe('Flat Blade mob skill', function()
    it('announces outside Trion, applies damage and Stun when processed, and shows Trion text', function()
        local blade = require('scripts/actions/mobskills/flat_blade')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local ready, params, damage, stun, text = nil, nil, nil, nil, nil
        local pool, processed = 1, false
        local mob = { getPool = function() return pool end, getWeaponDmg = function() return 77 end, messageBasic = function(_, ...) ready = { ... } end }
        local target = { takeDamage = function(_, ...) damage = { ... } end, showText = function(_, ...) text = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return processed end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) stun = { ... } end
        assert(blade.onMobSkillCheck(target, mob, {}) == 0 and ready[1] == xi.msg.basic.READIES_WS and ready[2] == 0 and ready[3] == 35)
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 123 and params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and stun == nil and text == nil)
        pool, processed, ready = xi.mobPool.QUBIA_ARENA_TRION, true, nil
        assert(blade.onMobSkillCheck(target, mob, {}) == 0 and ready == nil)
        blade.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(stun[1] == xi.effect.STUN and stun[2] == 1 and stun[3] == 0 and stun[4] == 4 and text[1] == mob and text[2] == zones[xi.zone.QUBIA_ARENA].text.FLAT_LAND)
    end)
end)
