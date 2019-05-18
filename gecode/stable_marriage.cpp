/**
 * Stable marriage problem
 */

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;


IntVar element_m2(Space & space, IntArgs c, int ix, IntVarArray y, int n) {
    IntVar ixx(space, ix, ix);
    IntVar y_ix(space, 0, n*n);
    element(space, y, ixx, y_ix);

    IntVar ix_n(space, 0, n*n);
    IntVar ix_n_plus_y_ix(space, 0, n*n);
    rel(space, ix_n == ixx * n);
    rel(space, ix_n_plus_y_ix == ix_n + y_ix);

    IntVar res(space, 0,n*n);
    element(space, c, ix_n_plus_y_ix, res);

    return res;
}


class StableMarriage : public Script {

    protected:
        IntVarArray wifes;
        IntVarArray husbands;

        static const int n = 9;

        static constexpr int _wrank_data[] = {
            3, 1, 5, 2, 8, 7, 6, 9, 4,
            9, 4, 8, 1, 7, 6, 3, 2, 5,
            3, 1, 8, 9, 5, 4, 2, 6, 7,
            8, 7, 5, 3, 2, 6, 4, 9, 1,
            6, 9, 2, 5, 1, 4, 7, 3, 8,
            2, 4, 5, 1, 6, 8, 3, 9, 7,
            9, 3, 8, 2, 7, 5, 4, 6, 1,
            6, 3, 2, 1, 8, 4, 5, 9, 7,
            8, 2, 6, 4, 9, 1, 3, 7, 5
        };

        static constexpr int _mrank_data[] = {
            7, 3, 8, 9, 6, 4, 2, 1, 5,
            5, 4, 8, 3, 1, 2, 6, 7, 9,
            4, 8, 3, 9, 7, 5, 6, 1, 2,
            9, 7, 4, 2, 5, 8, 3, 1, 6,
            2, 6, 4, 9, 8, 7, 5, 1, 3,
            2, 7, 8, 6, 5, 3, 4, 1, 9,
            1, 6, 2, 3, 8, 5, 4, 9, 7,
            5, 6, 9, 1, 2, 8, 4, 3, 7,
            6, 1, 4, 7, 5, 8, 3, 9, 2
        };

    public:

        StableMarriage(const Options& opt) : Script(opt),
            wifes{*this, n, 0, n - 1},
            husbands{*this, n, 0, n - 1} {

            IntArgs mranks{n * n, _mrank_data};
            IntArgs wranks{n * n, _wrank_data};

            //distinct(*this, wifes);
            //distinct(*this, husbands);

            for (int i = 0; i < n; i++) {
                rel(*this, element(husbands, wifes[i]) == i);
            }

            for (int i = 0; i < n; i++) {
                rel(*this, element(wifes, husbands[i]) == i);
            }

            for (int m = 0; m < n; m++) {
                IntVar rmen_res = element_m2(*this, mranks, m, wifes, n);
                for (int w = 0; w < n; w++) {
                    IntVar rwomen_res = element_m2(*this, wranks, w, husbands, n);
                    rel(*this, (mranks[m * n + w] < rmen_res) >> (rwomen_res < wranks[w * n + m]));
                }
            }

            for (int w = 0; w < n; w++) {
                IntVar rwomen_res = element_m2(*this, wranks, w, husbands, n);
                for (int m = 0; m < n; m++) {
                    IntVar rmen_res = element_m2(*this, mranks, m, wifes, n);
                    rel(*this, (wranks[w * n + m] < rwomen_res) >> (rmen_res < mranks[m * n + w]));
                }
            }

            channel(*this, wifes, husbands);

            branch(*this, wifes, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
            branch(*this, husbands, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
        }

        StableMarriage(StableMarriage& other) : Script(other) {
            wifes.update(*this, other.wifes);
            husbands.update(*this, other.husbands);
        }

        virtual void print(std::ostream& os) const {
            os << "Wifes: [";

            for (int i = 0; i < n; i++) {
                os << wifes[i].val() + 1 << " ";
            }

            os << "]" << std::endl << "Husbands: [";

            for (int i = 0; i < n; i++) {
                os << husbands[i].val() + 1 << " ";
            }
            os << "]" << std::endl;
        }

        virtual Space* copy(void) {
            return new StableMarriage(*this);
        }
};


int main(int argc, char *argv[]) {
    Options opt{"StableMarriage"};
    opt.solutions(0);
    opt.iterations(20'000);
    opt.parse(argc, argv);
    Script::run<StableMarriage, BAB, Options>(opt);

    return 0;
}
