#ifndef EASY_PLOT_HPP_INCLUDED
#define EASY_PLOT_HPP_INCLUDED

#include "GL/freeglut.h"
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdarg.h>

#include <iostream>

namespace easy_plot {
//------------------------------------------------------------------------------
    const int EASY_PLOT_DEF_WIDTH       = 480;  /**< Ширина по умолчанию */
    const int EASY_PLOT_DEF_WIDTH_STEP  = 4;    /**< Шаг ширины по умолчанию */
    const int EASY_PLOT_DEF_HEIGHT      = 240;  /**< Высота по умолчанию */
    const int EASY_PLOT_DEF_SCALE       = 1.1;  /**< Коэффициент отсутпа от нижней и верхней границы */
//------------------------------------------------------------------------------
    enum TypesErrors {
        EASY_PLOT_OK = 0,               ///< Ошибок нет
        EASY_PLOT_INVALID_PARAMETR = 1, ///< Неверно указан параметр
    };
//------------------------------------------------------------------------------
    enum PlotTypes {
       EASY_PLOT_1D = 0,
       EASY_PLOT_1D_SEVERAL = 1,
       EASY_PLOT_2D = 2,
    };
//------------------------------------------------------------------------------
    /** \brief Класс для хранения данных стиля линии
     */
    class LineSpec {
        public:
        double r, g, b, a;

        /** \brief Инициализация параметров линии по умолчанию
         * Данный конструктор инициализирует линию цветом по умолчанию
         */
        LineSpec() {
            r = 0.0;
            g = 1.0;
            b = 1.0;
            a = 1.0;
        }

        /** \brief Инициализация параметров линии
         * \param red уровеньк расного
         * \param green уровень зеленого
         * \param blue уровень голубого
         * \param alpha прозрачность
         */
        LineSpec(double red, double green, double blue, double alpha = 1.0) {
            r = red;
            g = green;
            b = blue;
            a = alpha;
        }
    };
//------------------------------------------------------------------------------
    class Drawing;
    std::vector<Drawing*> drawings;
    //static Drawing* current_instance;
//------------------------------------------------------------------------------
    /** \brief Класс для отрисовки графиков
     */
    class Drawing {
        private:
        std::vector<std::vector<double>> data;
        std::vector<LineSpec> line_style;
        double min_x, max_x, min_y, max_y;
        int mode = 0;
        bool is_init = false;
        bool is_window_init = false;
        public:
        static Drawing* current_instance;
        std::string window_name;    /**< Имя окна */
        int win_id = -1;

        /** \brief Инициализировать график
         * \param name имя окна
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1>
        void init(std::string name, std::vector<T1> &y, LineSpec style) {
            is_init = false;
            if(name == "" || y.size() < 2)
                return;

            line_style.resize(1);
            line_style[0] = style;

            data.resize(1);
            data[0].resize(y.size());
            for(size_t i = 0; i < y.size(); ++i) {
                data[0][i] = y[i];
            }

            max_y = *std::max_element(data[0].begin(), data[0].end());
            min_y = *std::min_element(data[0].begin(), data[0].end());

            mode = EASY_PLOT_1D;
            window_name = name;
            is_init = true;
        }

        /** \brief Конструктор класса графика
         * \param name имя окна
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1>
        Drawing(std::string name, std::vector<T1> &y, LineSpec style) {
            init(name, y, style);
        }

        /** \brief Инициализировать график
         * \param name имя окна
         * \param x вектор по оси X
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1, typename T2>
        void init(std::string name, std::vector<T1> &x, std::vector<T2> &y, LineSpec style) {
            is_init = false;
            if(name == "" || x.size() < 2 || y.size() < 2 || x.size() != y.size())
                return;

            line_style.resize(1);
            line_style[0] = style;

            data.resize(2);
            data[0].resize(x.size());
            data[1].resize(y.size());
            for(size_t i = 0; i < x.size(); ++i) {
                data[0][i] = x[i];
                data[1][i] = y[i];
            }

            max_x = *std::max_element(data[0].begin(), data[0].end());
            min_x = *std::min_element(data[0].begin(), data[0].end());
            max_y = *std::max_element(data[1].begin(), data[1].end());
            min_y = *std::min_element(data[1].begin(), data[1].end());

            mode = EASY_PLOT_2D;
            window_name = name;
            is_init = true;
        }

        /** \brief Нарисовать двумерный график
         * \param in вектор одномерного графика
         */
        template <typename T1, typename T2>
        Drawing(std::string name, std::vector<T1> &x, std::vector<T2> &y, LineSpec style) {
            init(name, x, y, style);
        }

        template <typename T1>
        void init(std::string name, std::vector<std::vector<T1>> &vec, std::vector<LineSpec> &style) {
            is_init = false;
            if(name == "" || vec.size() == 0 || style.size() != vec.size())
                return;

            if(vec[0].size() < 2)
                return;
            for(size_t i = 1; i < vec.size(); ++i) {
                if(vec[i].size() != vec[i - 1].size())
                    return;
            }

            line_style = style;

            data.resize(vec.size());
            for(size_t n = 0; n < vec.size(); ++n) {
                data[n].resize(vec[n].size());
                for(size_t i = 0; i < vec[n].size(); ++i) {
                    data[n][i] = vec[n][i];
                }
                if(n == 0) {
                    double pre_max_y = *std::max_element(data[n].begin(), data[n].end());
                    double pre_min_y = *std::min_element(data[n].begin(), data[n].end());
                    max_y = std::max(pre_max_y, max_y);
                    min_y = std::min(pre_min_y, min_y);
                } else {
                    max_y = *std::max_element(data[n].begin(), data[n].end());
                    min_y = *std::min_element(data[n].begin(), data[n].end());
                }
            }

            mode = EASY_PLOT_1D_SEVERAL;
            window_name = name;
            is_init = true;
        }

        /** \brief Нарисовать график
         * \param name имя окна
         * \param y
         * \param style стиль линии
         */
        template <typename T1>
        Drawing(std::string name, std::vector<std::vector<T1>> &vec, std::vector<LineSpec> &style) {
            init(name, vec, style);
        }

        /** \brief Функция рисования
         */
        void draw() {
            // рисуем график
            if(mode == EASY_PLOT_1D) {
                glClear(GL_COLOR_BUFFER_BIT);
                glBegin(GL_LINES);//начало рисования линий

                glColor3f(1, 1, 1);
                glVertex2f(0, 0);
                glVertex2f(data[0].size(), 0);

                glColor3f(line_style[0].r, line_style[0].g, line_style[0].b);
                for(size_t i = 0; i < data[0].size() - 1; ++i) {
                    glVertex2f((double)i, data[0][i]);
                    glVertex2f((double)i + 1.0, data[0][i + 1]);
                }
                glEnd();
                glFlush();
            } else
            // рисуем несколько графиков поверх друг друга
            if(mode == EASY_PLOT_1D_SEVERAL) {
                glClear(GL_COLOR_BUFFER_BIT);
                glBegin(GL_LINES);//начало рисования линий

                glColor3f(1, 1, 1);
                glVertex2f(0, 0);
                glVertex2f(data[0].size(), 0);

                for(size_t n = 0; n < data.size(); ++n) {
                    glColor3f(line_style[n].r, line_style[n].g, line_style[n].b);
                    for(size_t i = 0; i < data[n].size() - 1; ++i) {
                        glVertex2f((double)i, data[n][i]);
                        glVertex2f((double)i + 1.0, data[n][i + 1]);
                    }
                }
                glEnd();
                glFlush();
            } else
            // рисуем график заивисмости y от x
            if(mode == EASY_PLOT_2D) {
                glClear(GL_COLOR_BUFFER_BIT);
                glBegin(GL_LINES);

                glColor3f(1, 1, 1);
                glVertex2f(min_x, 0);
                glVertex2f(max_x, 0);

                glColor3f(line_style[0].r, line_style[0].g, line_style[0].b);
                for(size_t i = 0; i < data[0].size() - 1; ++i) {
                    glVertex2f(data[0][i], data[1][i]);
                    glVertex2f(data[0][i + 1], data[1][i + 1]);
                }
                glEnd();
                glFlush();
            }
        }

        static void update() {
            int win_id = glutGetWindow();
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->draw();
                    break;
                }
            }
        }

        /** \brief Обновить состояние окна
         */
        void update_window() {
            if(is_window_init) {
                //if(glutGetWindow() == win_id) {
                //std::cout << glutGetWindow() << " " << win_id << std::endl;
                //std::cout << current_instance << std::endl;
                // если окно инициализировано, вызываем неблокирующий glutMainLoop
                //glutSetWindow(win_id);
                //::glutDisplayFunc(update);
                   //glut
                glutMainLoopEvent();
                //}
                //glutMainLoopEvent();
                return;
            }
            // необходима инициализация окна
            if(mode == EASY_PLOT_1D || mode == EASY_PLOT_1D_SEVERAL) {
                glutInitWindowSize(
                    EASY_PLOT_DEF_WIDTH,
                    EASY_PLOT_DEF_HEIGHT);
                glutInitWindowPosition(0, 0);
                win_id = glutCreateWindow(window_name.c_str());
                glutSetWindow(win_id);

                glClearColor(0.0, 0.0, 0.0, 1.0);
                glMatrixMode(GL_PROJECTION); /*Настроим 2-х мерный вид*/
                glLoadIdentity();

                double offset = (max_y - min_y) * EASY_PLOT_DEF_SCALE;
                glOrtho(
                    0.0, (double)(data[0].size() - 1),
                    min_y - offset, max_y + offset,
                    -1.0, 1.0);

                //current_instance = this;
                ::glutDisplayFunc(update);
                glutMainLoopEvent();
            } else
            if(mode == EASY_PLOT_2D) {
                glutInitWindowSize(
                    EASY_PLOT_DEF_WIDTH,
                    EASY_PLOT_DEF_HEIGHT);
                glutInitWindowPosition(0, 0);
                win_id = glutCreateWindow(window_name.c_str());
                glutSetWindow(win_id);

                glClearColor(0.0, 0.0, 0.0, 1.0);
                glMatrixMode(GL_PROJECTION); /*Настроим 2-х мерный вид*/
                glLoadIdentity();

                double offset = (max_y - min_y) * EASY_PLOT_DEF_SCALE;
                glOrtho(
                    min_x, max_x,
                    min_y - offset, max_y + offset,
                    -1.0, 1.0);

                //current_instance = this;
                ::glutDisplayFunc(update);
                glutMainLoopEvent();
            }
            is_window_init = true;
        }
    };
//------------------------------------------------------------------------------
    std::mutex drawings_mutex;
//------------------------------------------------------------------------------
    /** \brief Инициализировать работу с графиками
     * \param argc
     * \param argv
     */
    void init(int *argc, char *argv[]) {
        std::thread glut_thread([&]() {
            // инициализируем FREEGLUT
            glutInit(argc, argv);
            // чтобы можно было закрывать окна
            glutSetOption(
                GLUT_ACTION_ON_WINDOW_CLOSE,
                GLUT_ACTION_CONTINUE_EXECUTION);
            // инициализируем режим отображения
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
            while(true) {
                // инициализируем все окна
                drawings_mutex.lock();
                for(size_t i = 0; i < drawings.size(); ++i) {
                    drawings[i]->update_window();
                    std::this_thread::yield();
                }
                drawings_mutex.unlock();
                std::this_thread::yield();
            }
        });
        glut_thread.detach();
    }
//------------------------------------------------------------------------------
    int get_pos_plot(std::string name) {
        for(size_t i = 0; i < drawings.size(); ++i) {
            if(drawings[i]->window_name == name) return i;
        }
        return -1;
    }
//------------------------------------------------------------------------------
    /** \brief Создает двухмерный линейный график данных в Y по сравнению с соответствующими значениями в X.
     * Функция графика отображает Y против X,
     * X и Y должны иметь одинаковую длину.
     * \param name имя окна
     * \param x вектор по оси X
     * \param y вектор по оси Y
     * \param style стиль линии
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1, typename T2>
    int plot(std::string name, std::vector<T1> &x, std::vector<T2> &y, LineSpec style = LineSpec()) {
        if(x.size() != y.size()) return EASY_PLOT_INVALID_PARAMETR;
        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, x, y, style);
        } else {
            drawings.push_back(new Drawing(name, x, y, style));
        }
        drawings_mutex.unlock();
    }
//------------------------------------------------------------------------------
    /** \brief Создает двухмерный линейный график данных в Y по сравнению с индексом каждого значения.
     * \param name имя окна
     * \param y вектор по оси Y
     * \param style стиль линии
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1>
    int plot(std::string name, std::vector<T1> &y, LineSpec style = LineSpec()) {
        if(y.size() <= 1)
            return EASY_PLOT_INVALID_PARAMETR;

        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, y, style);
        } else {
            drawings.push_back(new Drawing(name, y, style));
        }
        drawings_mutex.unlock();
    }
//------------------------------------------------------------------------------
    /** \brief Cтроит несколько векторов, используя одинаковые оси для всех линий
     * \param name имя окна
     * \param count количество векторов и стилей
     * \param ... перечисление векторов и стилей по порядку (y2, style2, y3, style2 и т.д.)
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1>
    int plot(std::string name, int count, ...) {
        va_list va;
        va_start(va, count);

        std::vector<std::vector<T1>> data;
        std::vector<LineSpec> line_style;
        for(int n = 0; n < count; ++n) {
            std::vector<T1> *vec_next = va_arg(va, std::vector<T1> *);
            LineSpec *style_next = va_arg(va, LineSpec *);
            data.push_back(*vec_next);
            line_style.push_back(*style_next);
        }
        va_end(va);

        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, data, line_style);
        } else {
            drawings.push_back(new Drawing(name, data, line_style));
        }
        drawings_mutex.unlock();
    }
//------------------------------------------------------------------------------
}

#endif // EASY_PLOT_HPP_INCLUDED
