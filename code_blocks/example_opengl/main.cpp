#include <iostream>
#include <cmath>
#include "easy_plot.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    ep::init(&argc, argv);
    cout << "Hello plot!" << endl;

    ep::WindowSpec wstyle0;
    wstyle0.is_zero_x_line = true;
    wstyle0.is_zero_y_line = true;
    std::vector<double> test1_x = {1.2,1,1,1,1.2,1,1};
    ep::plot("test1", wstyle0, test1_x);

    std::vector<double> test2_x = {0,2,3,4,2,0,1};
    ep::plot("test2", test2_x);


    ep::WindowSpec wstyle3;
    wstyle3.is_zero_x_line = true;
    wstyle3.is_zero_y_line = true;
    std::vector<double> test3_x = {-2,2,6,7,8,10,12};
    ep::plot("test3", wstyle3, test3_x, test2_x, ep::LineSpec(1,1,0));


    std::vector<double> test1_xx;
    for(int i = 0; i < 2000; ++i) {
        test1_xx.push_back(i%53);
    }
    ep::WindowSpec wstyle_xx;
    wstyle_xx.grid_period = 2.0/24.0;
    ep::plot<double>("test1-1", wstyle_xx, (int)1, test1_xx, ep::LineSpec(1,0,0));
    ep::plot<double>("test1-2", wstyle_xx, (int)1, test1_xx, ep::LineSpec(0,1,0));
    ep::plot<double>("test1-3", wstyle_xx, (int)1, test1_xx, ep::LineSpec(0,0,1));
    ep::plot<double>("test1-4", wstyle_xx, (int)1, test1_xx, ep::LineSpec(0,1,1));

    ep::WindowSpec wstyle;
    wstyle.grid_period = 2.0/24.0;
    ep::plot<double>(
        "test4",
        wstyle,
        (int)3,
        test1_x,
        ep::LineSpec(1,0,0),
        test2_x,
        ep::LineSpec(1,0,1),
        test3_x,
        ep::LineSpec(0,1,0));
    ep::save_image("test4", "test4.ppm");

    std::vector<double> line;
    for(int i = 0; i < 500; ++i) {
        line.push_back(sin(3.1415*(double)i/50));
    }
    std::vector<double> line05(line.size(), 0.5);
    std::vector<double> line06(line.size(), 0.6);
    ep::WindowSpec wstyle2;
    wstyle2.grid_period = 2.0/24.0;
    ep::plot<double>("test5", wstyle2, (int)3,
        line, ep::LineSpec(1,0,0),
        line05, ep::LineSpec(1,0,1),
        line06, ep::LineSpec(0,1,0));

    ep::save_image("test5", "test5.ppm");
    wstyle2.is_zero_x_line = true;

    // нарисуем изображение
    ep::WindowSpec image_wstyle;
    image_wstyle.is_grid = true;
    image_wstyle.height = 320;
    image_wstyle.width = 320;
    float image_data[32][32] = {};
    size_t image_ind = 0;
    for(size_t x = 0 ; x < 32; ++x) {
        for(size_t y = 0; y < 32; ++y, ++image_ind) {
            image_data[x][y] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18));
        }
    }

    image_wstyle.is_color_heatmap = false;
    ep::draw_heatmap("image_heatmap", image_wstyle, &image_data[0][0], 32, 32);
    image_wstyle.is_color_heatmap = true;
    ep::draw_heatmap("image_heatmap_color", image_wstyle, &image_data[0][0], 32, 32);
    image_wstyle.is_color_heatmap = true;
    image_wstyle.is_invert_color_heatmap = true;
    ep::draw_heatmap("image_heatmap_invert_color", image_wstyle, &image_data[0][0], 32, 32);

    // продолжим обновлять график

    // увеличим выборку и будем перерисовывать график некоторое время
    for(int step = 0; step < 1000; step++) {
        std::vector<double> line;
        for(int i = 0; i < 1000; ++i) {
            line.push_back(sin(3.1415*(double)(i + step*10)/50));
        }
        std::vector<double> line05(line.size(), -0.5);
        std::vector<double> line06(line.size(), 0.6);
        ep::plot<double>("test5", wstyle2, 3,
            line, ep::LineSpec(1,0,0),
            line05, ep::LineSpec(1,0,1),
            line06, ep::LineSpec(0,1,0));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    //while(true) {
    //    std::this_thread::yield();
    //}
    std::system("pause");
    return 0;
}
