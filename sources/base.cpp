#include "base.h"

Grid::Grid(void) { }
Grid::Grid(int x, int y) : x(x), y(y) { }

bool Choice::operator <(const Choice &c) const
{
	return prior >= c.prior;
}
