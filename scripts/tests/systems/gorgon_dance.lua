describe('Gorgon Dance mob skill',function()
 it('admits at 25 percent HP and forwards random gaze Petrification',function()
  local dance=require('scripts/actions/mobskills/gorgon_dance');local gaze,random=xi.mobskills.mobGazeMove,math.random;local args,message=nil,nil;local hpp=25;local mob={getHPP=function()return hpp end};local skill={setMsg=function(_,v)message=v end};assert(dance.onMobSkillCheck({},mob,skill)==0);hpp=26;assert(dance.onMobSkillCheck({},mob,skill)==1);hpp=25;xi.mobskills.mobGazeMove=function(...)args={...};return 456 end;math.random=function(min,max)assert(min==60 and max==180);return 120 end
  assert(dance.onMobWeaponSkill(mob,{},skill,{})==xi.effect.PETRIFICATION);xi.mobskills.mobGazeMove,math.random=gaze,random;assert(message==456 and args[3]==xi.effect.PETRIFICATION and args[4]==1 and args[5]==0 and args[6]==120)
 end)
end)
