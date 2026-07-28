local uragnite = require('scripts/mixins/families/uragnite')

describe('Uragnite mixin', function()
    it('enters shell only on qualifying physical hits and plans both states', function()
        assert(xi.mix.uragnite.shouldEnterShell(true, 20, 20, 0))
        assert(not xi.mix.uragnite.shouldEnterShell(false, 1, 20, 0))
        assert(not xi.mix.uragnite.shouldEnterShell(true, 21, 20, 0))
        assert(not xi.mix.uragnite.shouldEnterShell(true, 1, 20, 1))

        local entered = xi.mix.uragnite.enterPlan(0, 50, 250)
        assert(entered.animationSub == 1 and not entered.autoAttack and entered.damageTakenMod == -7500)
        assert(entered.regen == 50 and entered.skillList == 250 and entered.noMove == 1)

        local exited = xi.mix.uragnite.exitPlan(1, 50, 251)
        assert(exited.animationSub == 0 and exited.autoAttack and exited.damageTakenMod == -7500)
        assert(exited.regen == 50 and exited.skillList == 251 and exited.noMove == 0)
    end)
end)
