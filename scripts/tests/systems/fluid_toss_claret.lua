describe('Fluid Toss Claret mob skill', function()
    it('uses its critical physical plan and applies five-tick Poison only after processing', function()
        local toss=require('scripts/actions/mobskills/fluid_toss_claret');local move,process,status,random=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove,math.random;local params,damage,poison=nil,nil,nil
        local mob={getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end}
        xi.mobskills.mobPhysicalMove=function(_,_,_,_,value)params=value;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.SLASHING}end;xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(_,_,...)poison={...}end;math.random=function(min,max)assert(min==3 and max==6);return 5 end
        assert(toss.onMobSkillCheck(target,mob,{})==0 and toss.onMobWeaponSkill(mob,target,{},{})==123 and poison==nil)
        assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==1.5 and params.fTP[2]==1.5 and params.fTP[3]==1.5 and params.attackMultiplier[1]==2 and params.attackMultiplier[2]==2 and params.attackMultiplier[3]==2 and params.canCrit and params.criticalChance[1]==.10 and params.criticalChance[2]==.20 and params.criticalChance[3]==.25)
        xi.mobskills.processDamage=function()return true end;toss.onMobWeaponSkill(mob,target,{},{})
        xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove,math.random=move,process,status,random;assert(damage[1]==123 and poison[1]==xi.effect.POISON and poison[2]==100 and poison[3]==3 and poison[4]==15)
    end)
end)
