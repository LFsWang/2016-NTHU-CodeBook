#include<Geometry.cpp>
#include<iostream>
#include<cassert>
using namespace std;

int main()
{
    point<double> p(1,5),q(2,6);
    p=p+q;
    assert(p.x == 3);
    assert(p.y == 11);
    return 0;
}