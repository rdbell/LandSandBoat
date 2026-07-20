describe('Gas Shell mob skill',function()
 it('forwards Poison message with independently random power and duration',function()
  local shell=require('scripts/actions/mobskills/gas_shell');local status,random=xi.mobskills.mobStatusEffectMove,math.random;local calls,message={},nil;local skill={setMsg=function(_,v)message=v end};xi.mobskills.mobStatusEffectMove=function(_,_,effect,power,tick,duration)calls[#calls+1]={effect,power,tick,duration};return 456 end;math.random=function(min,max)if min==23 then assert(max==24);return 24 end;assert(min==30 and max==90);return 60 end
  assert(shell.onMobSkillCheck({}, {}, skill)==0 and shell.onMobWeaponSkill({}, {}, skill,{})==xi.effect.POISON);xi.mobskills.mobStatusEffectMove,math.random=status,random;assert(message==456 and #calls==1 and calls[1][1]==xi.effect.POISON and calls[1][2]==24 and calls[1][3]==0 and calls[1][4]==60)
 end)
end)
