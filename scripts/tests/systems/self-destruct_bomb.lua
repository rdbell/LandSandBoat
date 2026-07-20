require('scripts/actions/mobskills/self-destruct_bomb')
describe('Self-Destruct Bomb mob skill', function()
    it('uses fire magical plan capped by mob HP and finalizes setHP 0', function()
        local bomb = require('scripts/actions/mobskills/self-destruct_bomb')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local random = math.random
        local params, damage, hp = nil, nil, 500
        local mob = {
            getHP = function() return 500 end,
            setHP = function(_, v) hp = v end,
        }
        local target = {
            getMaxHP = function() return 1000 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        math.random = function(a, b)
            assert(a == 0.7 and b == 1.1)
            return 0.8
        end
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 800, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(bomb.onMobSkillCheck(target, mob, {}) == 0 and bomb.onMobWeaponSkill(mob, target, {}, {}) == 800)
        assert(params.baseDamage == 500 and params.skipMagicBonusDiff and params.element == xi.element.FIRE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        bomb.onMobWeaponSkill(mob, target, {}, {})
        assert(damage[1] == 800)
        bomb.onMobSkillFinalize(mob, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        math.random = random
        assert(hp == 0)
    end)
end)
