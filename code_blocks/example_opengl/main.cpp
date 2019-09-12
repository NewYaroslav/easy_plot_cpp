#include <iostream>
#include <cmath>
#include "easy_plot.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    easy_plot::init(&argc, argv);
    cout << "Hello plot!" << endl;

    easy_plot::WindowSpec wstyle0;
    wstyle0.is_zero_x_line = true;
    wstyle0.is_zero_y_line = true;
    std::vector<double> test1_x = {1.2,1,1,1,1.2,1,1};
    easy_plot::plot("test1", wstyle0, test1_x);

    std::vector<double> test2_x = {0,2,3,4,2,0,1};
    easy_plot::plot("test2", test2_x);


    easy_plot::WindowSpec wstyle3;
    wstyle3.is_zero_x_line = true;
    wstyle3.is_zero_y_line = true;
    std::vector<double> test3_x = {-2,2,6,7,8,10,12};
    easy_plot::plot("test3", wstyle3, test3_x, test2_x, easy_plot::LineSpec(1,1,0));


    std::vector<double> test1_xx;
    for(int i = 0; i < 2000; ++i) {
        test1_xx.push_back(i%53);
    }
    easy_plot::WindowSpec wstyle_xx;
    wstyle_xx.grid_period = 2.0/24.0;
    easy_plot::plot<double>("test1-1", wstyle_xx, (int)1, test1_xx, easy_plot::LineSpec(1,0,0));
    easy_plot::plot<double>("test1-2", wstyle_xx, (int)1, test1_xx, easy_plot::LineSpec(0,1,0));
    easy_plot::plot<double>("test1-3", wstyle_xx, (int)1, test1_xx, easy_plot::LineSpec(0,0,1));
    easy_plot::plot<double>("test1-4", wstyle_xx, (int)1, test1_xx, easy_plot::LineSpec(0,1,1));

    easy_plot::WindowSpec wstyle;
    wstyle.grid_period = 2.0/24.0;
    easy_plot::plot<double>("test4", wstyle, (int)3, test1_x, easy_plot::LineSpec(1,0,0), test2_x, easy_plot::LineSpec(1,0,1), test3_x, easy_plot::LineSpec(0,1,0));
    easy_plot::save_image("test4", "test4.ppm");

    std::vector<double> line;
    for(int i = 0; i < 500; ++i) {
        line.push_back(sin(3.1415*(double)i/50));
    }
    std::vector<double> line05(line.size(), 0.5);
    std::vector<double> line06(line.size(), 0.6);
    easy_plot::WindowSpec wstyle2;
    wstyle2.grid_period = 2.0/24.0;
    easy_plot::plot<double>("test5", wstyle2, (int)3,
        line, easy_plot::LineSpec(1,0,0),
        line05, easy_plot::LineSpec(1,0,1),
        line06, easy_plot::LineSpec(0,1,0));

    easy_plot::save_image("test5", "test5.ppm");

    wstyle2.is_zero_x_line = true;
    // увеличим выборку и будем перерисовывать график некоторое время
    for(int step = 0; step < 1000; step++) {
        std::vector<double> line;
        for(int i = 0; i < 1000; ++i) {
            line.push_back(sin(3.1415*(double)(i + step*10)/50));
        }
        std::vector<double> line05(line.size(), -0.5);
        std::vector<double> line06(line.size(), 0.6);
        easy_plot::plot<double>("test5", wstyle2, 3,
            line, easy_plot::LineSpec(1,0,0),
            line05, easy_plot::LineSpec(1,0,1),
            line06, easy_plot::LineSpec(0,1,0));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    while(true) {

        std::this_thread::yield();
    }
    return 0;
}
