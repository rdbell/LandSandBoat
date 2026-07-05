local runner = require('scripts.tests.packets.s2c.0x028_battle2.base')
local suite = require('scripts.tests.packets.s2c.0x028_battle2.magic')

runner.runSuite('Magic #magic', suite)
