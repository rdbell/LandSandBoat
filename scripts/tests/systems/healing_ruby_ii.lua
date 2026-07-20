describe('Healing Ruby II mob skill',function()
 it('defaults potency, subtracts a random quarter-range amount, and heals its caster',function()
  local ruby=require('scripts/actions/mobskills/healing_ruby_ii');local heal,random=xi.mobskills.mobHealMove,math.random;local receiver,amount,message=nil,nil,nil;local param,max=0,3.75;local mob={getMaxHP=function()return 1000 end};local target={};local skill={getParam=function()return param end,setMsg=function(_,value)message=value end};math.random=function(min,value)assert(min==0 and value==max);return 2 end;xi.mobskills.mobHealMove=function(who,value)receiver,amount=who,value;return 456 end
  assert(ruby.onMobSkillCheck(target,mob,skill)==0 and ruby.onMobWeaponSkill(mob,target,skill,{})==456);assert(receiver==mob and amount==130 and message==xi.msg.basic.SELF_HEAL);param,max=20,5;ruby.onMobWeaponSkill(mob,target,skill,{});math.random,xi.mobskills.mobHealMove=random,heal;assert(amount==180)
 end)
end)
