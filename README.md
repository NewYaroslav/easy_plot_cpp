# easy_plot_cpp
Простая библиотека C++ для рисования графиков 

### Описание

Данная библиотека позволяет рисовать простые графики.
Пример:
![example_0 example](img/example_0.png)

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

	// выводим три графика в одном
    easy_plot::plot<double>("Y1Y2Y3", 3, x, easy_plot::LineSpec(1,0,0), x2, easy_plot::LineSpec(1,0,1), y, easy_plot::LineSpec(0,1,0));

    while(true) {
        std::this_thread::yield();
    }
    return 0;
}
```

### Полезные ссылки

* *GLUT* (https://www.opengl.org/resources/libraries/glut/)[https://www.opengl.org/resources/libraries/glut/]
* *FreeGLUT* (http://freeglut.sourceforge.net/)[http://freeglut.sourceforge.net/]
* *GL2PS* (http://www.geuz.org/gl2ps/)[http://www.geuz.org/gl2ps/]
* *GL2PS gitlab* (https://gitlab.onelab.info/gl2ps/gl2ps)[https://gitlab.onelab.info/gl2ps/gl2ps]
