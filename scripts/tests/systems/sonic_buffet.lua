require('scripts/actions/mobskills/sonic_buffet')
describe('Sonic Buffet mob skill', function()
    it('uses wind magical plan and dispels 2-3 effects after processing', function()
        local buffet = require('scripts/actions/mobskills/sonic_buffet')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local random = math.random
        local params, damage, dispels = nil, nil, 0
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function() dispels = dispels + 1; return 1 end,
        }
        math.random = function(a, b)
            assert(a == 2 and b == 3)
            return 2
        end
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(buffet.onMobSkillCheck(target, mob, {}) == 0 and buffet.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.50 and damage == nil and dispels == 0)
        xi.mobskills.processDamage = function() return true end
        buffet.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        math.random = random
        assert(damage[1] == 80 and dispels == 2)
    end)
end)
