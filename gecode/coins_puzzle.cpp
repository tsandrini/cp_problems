/**
 * Coins puzzle
 *
 * -----------------------------------------
 *
 */

#include <gecode/int.hh>
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>

#define INITIAL_DIST 10'000'000

using namespace Gecode;

class CoinsPuzzle : public IntMinimizeScript {

    protected:
        IntVarArray x;
        IntVar z;

        static const int n = 7;
        static const int c = 3;

    public:

        CoinsPuzzle(const Options& opt) : IntMinimizeScript(opt),
            x{*this, n * n, 0, 1},
            z{*this, 0, INITIAL_DIST} {

            Matrix<IntVarArgs> m{x, n, n};

            // There has to be exactly 'c' coins in every col and row
            for (int i = 0; i < n; i++) {
                linear(*this, m.col(i), IRT_EQ, c);
                linear(*this, m.row(i), IRT_EQ, c);
            }

            IntVarArray dists{*this, n * n, 0, INITIAL_DIST};
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    const int d = abs(i - j);
                    rel(*this, dists[n * i + j] == m(i, j) * d * d);
                }
            }

            linear(*this, dists, IRT_EQ, z);
            branch(*this, x, INT_VAR_REGRET_MAX_MIN(), INT_VAL_SPLIT_MIN());
        }

        virtual void print(std::ostream& os) const {
            os << "z: " << z << std::endl;
            Matrix<IntVarArray> m{x, n, n};

            for (int i = 0; i < n; i++) {
                os << "\t";
                for (int j = 0; j < n; j++) {
                    os << m(i, j) << "";
                }
                os << std::endl;
            }
        }

        virtual IntVar cost(void) const {
            return z;
        }

        CoinsPuzzle(CoinsPuzzle& other) : IntMinimizeScript(other) {
            x.update(*this, other.x);
            z.update(*this, other.z);
        }

        virtual Space* copy(void) {
            return new CoinsPuzzle(*this);
        }
};

int main(int argc, char *argv[]) {
    Options opt{"CoinsPuzzle"};
    opt.solutions(0);
    opt.iterations(20'000);
    opt.parse(argc, argv);

    IntMinimizeScript::run<CoinsPuzzle, BAB, Options>(opt);

    return 0;
}
