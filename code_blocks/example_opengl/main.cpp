#include <iostream>
#include "easy_plot.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Hello world!" << endl;

    easy_plot::init(&argc, argv);
    std::vector<double> test1_x = {0,1,0,1,2,0,1};
    easy_plot::plot("test1", test1_x);

    std::vector<double> test2_x = {0,2,3,4,2,0,1};
    easy_plot::plot("test2", test2_x);

    std::vector<double> test3_x = {0,2,6,7,8,10,12};
    easy_plot::plot("test3", test3_x, test2_x, easy_plot::LineSpec(1,1,0));

    easy_plot::plot<double>("test4", 3, test1_x, easy_plot::LineSpec(1,0,0), test2_x, easy_plot::LineSpec(1,0,1), test3_x, easy_plot::LineSpec(0,1,0));

    while(true) {
        std::this_thread::yield();
    }
    return 0;
}
