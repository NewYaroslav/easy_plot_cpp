# easy_plot_cpp
Простая библиотека C++ для рисования графиков и тепловых карт

### Описание

Данная *header-only* библиотека позволяет рисовать простые графики или тепловые карты сразу из программы.

Пример:
![example_0 example](img/example_0.png)
![example_heat_map](img/example_heat_map.png)![example_monochrome_image](img/example_monochrome_image.png)

Отличия от других библиотек и особенности:

* Для работы нужен только *FreeGLUT*, не нужно устанавливать громоздкий *OpenCV* или быть зависимым от *matplotlib*
* Библиотека содержит мало кода, помещается в один *header* файл, ее можно легко доработать под свои нужды
* Рисование графиков происходит в отдельном потоке, можно перерисовывать графики
* Можно рисовать тепловые карты
* Можно использовать курсор мыши для получения данных графика или тепловой карты

### Как установить?

* Инструкция для *Code::Blocks* и компилятора *mingw*

Подключить библиотеку *freeglut* или *freeglut_static* (во втором случае также установите макрос FREEGLUT_STATIC), а также *opengl32*, *winmm*, *gdi32*. 
Подключить в проекте заголовочный файл *include/easy_plot.hpp*
Указать С++11 или выше

Для проверки можно запустить код:

```C++
#include "easy_plot.hpp"

int main(int argc, char* argv[]) {
    easy_plot::init(&argc, argv);
	
    std::vector<double> x = {0,1,0,1,2,0,1};
    easy_plot::plot("X", x);

    // ставим красный цвет линии
    std::vector<double> y = {0,2,3,4,2,0,1};
    easy_plot::plot("Y", y, easy_plot::LineSpec(1,0,0));

	
    std::vector<double> x2 = {0,2,6,7,8,10,12};
    easy_plot::plot("XY", x2, y, easy_plot::LineSpec(1,1,0));

    easy_plot::WindowSpec wstyle; // тут можно настроить стиль графика (цвет фона и пр.)
    // выводим три графика в одном
    easy_plot::plot<double>("Y1Y2Y3", wstyle, 3, x, easy_plot::LineSpec(1,0,0), x2, easy_plot::LineSpec(1,0,1), y,     easy_plot::LineSpec(0,1,0));

    while(true) {
        std::this_thread::yield();
    }
    return 0;
}
```

Чтобы нарисовать тепловую карту:

```C++
#include "easy_plot.hpp"

int main(int argc, char* argv[]) {
    easy_plot::init(&argc, argv);
	
    easy_plot::WindowSpec image_wstyle;
    image_wstyle.is_grid = true;
    image_wstyle.height = 320;
    image_wstyle.width = 320;
    float image_data[32*32] = {};
    size_t image_ind = 0;
    for(size_t x = 0 ; x < 32; ++x) {
        for(size_t y = 0; y < 32; ++y, ++image_ind) {
            image_data[image_ind] = 1024 - std::sqrt((x - 15) * (x - 15) + (y - 15) * (y - 15));
        }
    }

    image_wstyle.is_heat_map = false;
    easy_plot::draw_image("image_test", image_wstyle, image_data, 32, 32);
    image_wstyle.is_heat_map = true;
    easy_plot::draw_image("image_heat_map", image_wstyle, image_data, 32, 32);

    while(true) {
        std::this_thread::yield();
    }
    return 0;
}
```

Программа нарисует такую тепловую карту:

![example_heat_map](img/example_heat_map_2.png)

### Как пользоваться

* Для рисования графика вызовите подходящую для ваших нужд *easy_plot::plot* функцию
* Функция *plot* может принимать такие параметры, как имя окна, стиль окна (различные настройки цвета и пр., см. *WindowSpec*), данные графиков и стиль линий. 
* Если графиков несколько, изначально они будут расположены по всему экрану равномерно, как на картинке
![example_1 example1](img/example_1.png)
* Если навести курсор мыши на график, можно узнать номер линии и данные по осям X и Y
![example_2 example2](img/example_2.png)
### Полезные ссылки

* *GLUT* (https://www.opengl.org/resources/libraries/glut/)[https://www.opengl.org/resources/libraries/glut/]
* *FreeGLUT* (http://freeglut.sourceforge.net/)[http://freeglut.sourceforge.net/]
* *GL2PS* (http://www.geuz.org/gl2ps/)[http://www.geuz.org/gl2ps/]
* *GL2PS gitlab* (https://gitlab.onelab.info/gl2ps/gl2ps)[https://gitlab.onelab.info/gl2ps/gl2ps]
