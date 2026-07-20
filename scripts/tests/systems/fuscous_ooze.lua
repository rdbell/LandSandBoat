describe('Fuscous Ooze mob skill',function()
 it('shares a random duration across Weight and Encumbrance II, forwards Weight message, and returns USES',function()
  local ooze=require('scripts/actions/mobskills/fuscous_ooze');local status,random=xi.mobskills.mobStatusEffectMove,math.random;local effects,message=nil,nil;local mob={};local target={};local skill={setMsg=function(_,v)message=v end}
  xi.mobskills.mobStatusEffectMove=function(_,_,effect,power,tick,duration)effects=effects or {};effects[#effects+1]={effect,power,tick,duration};return 456 end;math.random=function(min,max)assert(min==30 and max==45);return 37 end
  local result=ooze.onMobWeaponSkill(mob,target,skill,{})
  xi.mobskills.mobStatusEffectMove,math.random=status,random;assert(result==xi.msg.basic.USES and message==456 and #effects==2);assert(effects[1][1]==xi.effect.WEIGHT and effects[1][2]==50 and effects[1][3]==0 and effects[1][4]==37 and effects[2][1]==xi.effect.ENCUMBRANCE_II and effects[2][2]==0xFFFF and effects[2][3]==0 and effects[2][4]==37)
 end)
end)
