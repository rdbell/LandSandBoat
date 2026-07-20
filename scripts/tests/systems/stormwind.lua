require('scripts/actions/mobskills/stormwind')
describe('Stormwind mob skill', function()
    it('uses wind magical plan with Kreutzet fTP scaling', function()
        local wind = require('scripts/actions/mobskills/stormwind')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, pool, counter = nil, nil, 0, 0
        local mob = {
            getMainLvl = function() return 50 end,
            getPool = function() return pool end,
            getLocalVar = function(_, k) assert(k == 'stormwindDamage'); return counter end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(wind.onMobSkillCheck(target, mob, {}) == 0 and wind.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 3.0 and damage == nil)
        pool = xi.mobPool.KREUTZET; counter = 2
        wind.onMobWeaponSkill(mob, target, {}, {})
        assert(params.fTP[1] == 3.25)
        counter = 3
        wind.onMobWeaponSkill(mob, target, {}, {})
        assert(params.fTP[1] == 3.60)
        xi.mobskills.processDamage = function() return true end
        wind.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
