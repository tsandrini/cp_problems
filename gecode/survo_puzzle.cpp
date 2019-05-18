/**
 * Survo puzzle
 *
 * http://en.wikipedia.org/wiki/Survo_Puzzle
 * -----------------------------------------
 *
 * Problem took from http://www.hakank.org/gecode/ -- survo_puzzle.cpp
 */

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;


class SurvoPuzzle : public Script {

    protected:
        IntVarArray x;
        static const int rows = 3;
        static const int cols = 6;

    public:

        SurvoPuzzle(const Options& opt) : Script(opt),
            x{*this, rows * cols, 1, rows * cols} {

            const int _rowsums[] = {30, 86, 55};
            const int _colsums[] = {22, 11, 42, 32, 27, 37};
            const int _m[rows][cols] = {
                {0, 0,  0, 0, 0, 0},
                {0, 0, 18, 0, 0, 0},
                {0, 0,  0, 0, 0, 0},
            };

            Matrix<IntVarArgs> m{x, cols, rows};

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (_m[i][j] > 0) {
                        rel(*this, m(j, i), IRT_EQ, _m[i][j]);
                    }
                }
            }

            distinct(*this, x);

            for (int i = 0; i < rows; i++) {
                linear(*this, m.row(i), IRT_EQ, _rowsums[i]);
            }

            for (int j = 0; j < cols; j++) {
                linear(*this, m.col(j), IRT_EQ, _colsums[j]);
            }

            branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MAX());
        }

        virtual void print(std::ostream& os) const {
            Matrix<IntVarArray> m{x, cols, rows};

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    os.width(3);
                    os << m(j, i) << " ";
                }
                os << std::endl;
            }

            os << std::endl;
        }

        SurvoPuzzle(SurvoPuzzle& other) : Script(other) {
            x.update(*this, other.x);
        }

        virtual Space* copy(void) {
            return new SurvoPuzzle(*this);
        }
};

int main(int argc, char *argv[]) {

    Options opt{"SurvoPuzzle"};
    opt.solutions(0);
    opt.iterations(20'000);
    opt.parse(argc, argv);
    Script::run<SurvoPuzzle, DFS, Options>(opt);

    return 0;
}
