require('scripts/actions/mobskills/aqua_ball')
describe('Aqua Ball mob skill', function()
    it('uses Water magical parameters and damages and applies STR Down only after processing succeeds', function()
        local move,process,effect=xi.mobskills.mobMagicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove; local params,damage,applied=nil,nil,nil
        local mob={getMainLvl=function() return 50 end}; local target={takeDamage=function(_,...) damage={...} end}
        xi.mobskills.mobMagicalMove=function(_,_,_,_,p) params=p;return {damage=123,attackType=xi.attackType.MAGICAL,damageType=xi.damageType.WATER} end; xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(...)applied={...}end
        local ball=require('scripts/actions/mobskills/aqua_ball');assert(ball.onMobSkillCheck(target,mob,{})==0 and ball.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==52 and params.fTP[1]==2 and params.fTP[2]==2.5 and params.fTP[3]==3 and damage==nil and applied==nil)
        xi.mobskills.processDamage=function()return true end;ball.onMobWeaponSkill(mob,target,{},{});xi.mobskills.mobMagicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,effect;assert(damage[1]==123 and applied[3]==xi.effect.STR_DOWN and applied[4]==10 and applied[5]==9 and applied[6]==180)
    end)
end)
