local khimaira = require('scripts/mixins/families/khimaira')

describe('Khimaira mixin', function()
    it('drops raised wings only on an inclusive five-percent critical roll', function()
        assert(xi.mix.khimaira.shouldDropWings(5, 0))
        assert(not xi.mix.khimaira.shouldDropWings(6, 0))
        assert(not xi.mix.khimaira.shouldDropWings(1, 1))
    end)
end)
