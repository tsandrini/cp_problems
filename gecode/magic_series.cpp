/**
 * Magic series
 *
 */

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;


class MagicSeries : public Script {

    protected:
        IntVarArray x;
        static const int n = 5;

    public:

        MagicSeries(const Options& opt) : Script(opt), x{*this, n, 0, n - 1} {

            rel(*this, x[0], IRT_NQ, 0); // First number cannot be zero

            for (int i = 0; i < n; i++) {
                BoolVarArgs occurences(*this, n, 0, 1);

                for (int j = 0; j < n; j++) {
                    rel(*this, x[j], IRT_EQ, i, occurences[j]);
                }

                linear(*this, occurences, IRT_EQ, x[i]);
            }

            branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MAX());
        }

        virtual void print(std::ostream& os) const {
            for (int i = 0; i < n; i++) {
                os.width(2);
                os << i << " ";
            }
            os << std::endl;

            for (int i = 0; i < n; i++) {
                os.width(2);
                os << x[i] << " ";
            }
            os << std::endl;
        }

        MagicSeries(MagicSeries& other) : Script(other) {
            x.update(*this, other.x);
        }

        virtual Space* copy(void) {
            return new MagicSeries(*this);
        }
};

int main(int argc, char *argv[]) {
    Options opt{"MagicSeries"};
    opt.solutions(0);
    opt.iterations(20'000);
    opt.parse(argc, argv);
    Script::run<MagicSeries, DFS, Options>(opt);

    return 0;
}
