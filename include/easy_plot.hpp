#ifndef EASY_PLOT_HPP_INCLUDED
#define EASY_PLOT_HPP_INCLUDED

#include "GL/freeglut.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdarg.h>
#include <numeric>

#include <iostream>

namespace easy_plot {
//------------------------------------------------------------------------------
    const int EASY_PLOT_DEF_WIDTH       = 480;  /**< Ширина по умолчанию */
    const int EASY_PLOT_DEF_HEIGHT      = 240;  /**< Высота по умолчанию */
    const double EASY_PLOT_DEF_INDENT   = 0.1;  /**< Размер отсутпа от границы окна */
//------------------------------------------------------------------------------
    enum TypesErrors {
        EASY_PLOT_OK = 0,               ///< Ошибок нет
        EASY_PLOT_INVALID_PARAMETR = 1, ///< Неверно указан параметр
    };
//------------------------------------------------------------------------------
    enum PlotTypes {
       EASY_PLOT_NO_INIT = -1,
       EASY_PLOT_1D = 0,
       EASY_PLOT_1D_SEVERAL = 1,
       EASY_PLOT_2D = 2,
    };
//------------------------------------------------------------------------------
    /** \brief Класс для хранения данных стиля окна
     */
    class WindowSpec {
        public:
        int width = EASY_PLOT_DEF_WIDTH;        /**< Шириная окна графика */
        int height = EASY_PLOT_DEF_HEIGHT;      /**< Высота окна графика */
        double indent = EASY_PLOT_DEF_INDENT;   /**< Отступ от края окна */
        //@{
        /** цвет фона (R G B) */
        double br = 0.0, bg = 0.0, bb = 0.0;
        //@}
        //@{
        /** цвет сетки (R G B) */
        double gr = 0.1, gg = 0.1, gb = 0.1;
        //@}
        //@{
        /** цвет рамки (R G B) */
        double fr = 1.0, fg = 1.0, fb = 1.0;
        //@}
        bool is_zero_x_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_zero_y_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_grid = true;
        double grid_period = 0.1;               /**< Период сетки */

        /** \brief Инициализация параметров Окна по умолчанию
         */
        WindowSpec() { };
    };
//------------------------------------------------------------------------------
    /** \brief Класс для хранения данных стиля линии
     */
    class LineSpec {
        public:
        //@{
        /** цвет линии (R G B) */
        double r = 0.0, g = 1.0, b = 1.0, a = 1.0;
        //@}

        /** \brief Инициализация параметров линии по умолчанию
         * Данный конструктор инициализирует линию цветом по умолчанию
         */
        LineSpec() {};

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
    static std::vector<Drawing*> drawings;
//------------------------------------------------------------------------------
    /** \brief Класс для отрисовки графиков
     */
    class Drawing {
        private:
        WindowSpec window_style;                        /**< Стиль окна */
        std::vector<std::vector<double>> raw_data_y;    /**< Данные для рисования всех графиков */
        std::vector<std::vector<double>> raw_data_x;    /**< Данные для рисования всез графиков */
        std::vector<LineSpec> line_style;               /**< Стили линий для всех графиков */
        std::atomic<bool> is_raw_data;
        double min_x, max_x, min_y, max_y;              /**< Минимумы и максимумы данных для рисования графиков */
        std::atomic<bool> is_window_init;
        /** \brief Получить нормализованное значение Y
         * \param ny не нормализованное значение
         * \return Нормализованное значение Y
         */
        inline double get_y(double ny) {
            if(max_y == min_y) return 0.0;
            return ((ny - min_y) / (max_y - min_y)) * 2.0 - 1.0;
        }

        /** \brief Получить нормализованное значение X
         * \param nx не нормализованное значение
         * \return Нормализованное значение X
         */
        inline double get_x(double nx) {
            if(max_x == min_x) return 0.0;
            return ((nx - min_x) / (max_x - min_x)) * 2.0 - 1.0;
        }

        /** \brief Получить значения оси X
         * \param size размер массива
         * \return данные оси X
         */
        std::vector<std::vector<int>> get_x_axis_values(size_t size) {
            std::vector<int> x(size);
            std::iota(std::begin(x), std::end(x), 0);
            std::vector<std::vector<int>> temp;
            temp.push_back(x);
            return temp;
        }

        /** \brief Получить значения оси X
         * \param size размер массива
         * \return данные оси X
         */
        template<typename T1>
        std::vector<std::vector<int>> get_x_axis_values(std::vector<std::vector<T1>> & data) {
            std::vector<std::vector<int>> temp(data.size());
            for(size_t  i = 0; i < data.size(); ++i) {
                temp[i].resize(data[i].size());
                std::iota(std::begin(temp[i]), std::end(temp[i]), 0);

            }
            return temp;
        }

        template<typename T1>
        std::vector<std::vector<T1>> get_axis_values(std::vector<T1> &data) {
            std::vector<std::vector<T1>> temp;
            temp.push_back(data);
            return temp;
        }

        template<typename T1>
        std::vector<T1> get_style_values(T1 &data) {
            std::vector<T1> temp;
            temp.push_back(data);
            return temp;
        }

        void set_start_state() {
            is_raw_data = false;
            is_window_init = false;
        }

        public:
        static Drawing* current_instance;       /**< Указатель на класс */
        std::string window_name;                /**< Имя окна графика */
        int win_id = -1;                        /**< Униклаьный ID графика */

        Drawing() {
            set_start_state();
        };

        /** \brief Инициализация графика
         * \param name имя окна
         * \param wstyle парамтеры стиля окна
         * \param x значения графика X
         * \param y значения графика Y
         * \param styles стили линий
         */
        template <typename T1, typename T2>
        void init(std::string name, WindowSpec wstyle, std::vector<std::vector<T1>> &x, std::vector<std::vector<T2>> &y, std::vector<LineSpec> &styles) {
            is_raw_data = false;
            if(x.size() != y.size() || styles.size() != y.size() || name == "" || x[0].size() == 0 || y[0].size() == 0) return;
            max_y = y[0][0];
            min_y = y[0][0];
            max_x = x[0][0];
            min_x = x[0][0];

            raw_data_x.resize(x.size());
            raw_data_y.resize(y.size());
            for(size_t i = 0; i < x.size(); ++i) {
                if(x[i].size() != y[i].size()) return;
                raw_data_x[i].resize(x[i].size());
                raw_data_y[i].resize(y[i].size());
                for(size_t j = 0; j < x[i].size(); ++j) {
                    raw_data_x[i][j] = x[i][j];
                    raw_data_y[i][j] = y[i][j];
                    max_y = std::max((double)y[i][j], max_y);
                    min_y = std::min((double)y[i][j], min_y);
                    max_x = std::max((double)x[i][j], max_x);
                    min_x = std::min((double)x[i][j], min_x);
                } // for j
            } // for i
            window_name = name;
            window_style = wstyle;
            line_style = styles;
            is_raw_data = true;
            if(is_window_init) glutPostRedisplay(); //is_redraw = true;
        }

        /** \brief Инициализация графика
         * \param name имя окна
         * \param wstyle стиль окна
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1>
        void init(std::string name, WindowSpec wstyle, std::vector<T1> &y, LineSpec style) {
            std::vector<LineSpec> vstyle = get_style_values(style);
            std::vector<std::vector<int>> vx = get_x_axis_values(y.size());
            std::vector<std::vector<T1>> vy = get_axis_values(y);
            init(name, wstyle, vx, vy, vstyle);
        }

        template <typename T1>
        Drawing(std::string name, WindowSpec wstyle, std::vector<T1> &y, LineSpec style) {
            set_start_state();
            init(name, wstyle, y, style);
        };

        /** \brief Инициализация графика
         * \param name имя окна
         * \param wstyle стиль окна
         * \param x вектор по оси X
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1, typename T2>
        void init(std::string name, WindowSpec wstyle, std::vector<T1> &x, std::vector<T2> &y, LineSpec style) {
            std::vector<LineSpec> vstyle = get_style_values(style);
            std::vector<std::vector<T1>> vx = get_axis_values(x);
            std::vector<std::vector<T2>> vy = get_axis_values(y);
            init(name, wstyle, vx, vy, vstyle);
        }

        template <typename T1, typename T2>
        Drawing(std::string name, WindowSpec wstyle, std::vector<T1> &x, std::vector<T2> &y, LineSpec style) {
            set_start_state();
            init(name, wstyle, x, y, style);
        };

        /** \brief Инициализация графика
         * \param name имя окна
         * \param y
         * \param style стиль линии
         */
        template <typename T1>
        void init(std::string name, WindowSpec wstyle, std::vector<std::vector<T1>> &vec, std::vector<LineSpec> &styles) {
            std::vector<std::vector<int>> vx = get_x_axis_values(vec);
            init(name, wstyle, vx, vec, styles);
        }

        template <typename T1>
        Drawing(std::string name, WindowSpec wstyle, std::vector<std::vector<T1>> &vec, std::vector<LineSpec> &styles) {
            set_start_state();
            init(name, wstyle, vec, styles);
        };

        /** \brief Функция рисования графика
         */
        void draw() {
            if(!is_raw_data) return;
            glClear(GL_COLOR_BUFFER_BIT);
            glBegin(GL_LINES);//начало рисования линий
            if(window_style.is_zero_x_line) {
                glColor3f(window_style.fr, window_style.fg, window_style.fb);
                glVertex2f(-1, get_y(0));
                glVertex2f(1, get_y(0));
            }
            if(window_style.is_zero_y_line) {
                glColor3f(window_style.fr, window_style.fg, window_style.fb);
                glVertex2f(get_x(0), -1);
                glVertex2f(get_x(0), 1);
            }
            if(window_style.is_grid && window_style.grid_period != 0.0) {
                glColor3f(window_style.gr, window_style.gg, window_style.gb);
                for(double x = -1.0; x < 1.0; x += window_style.grid_period) {
                    glVertex2f(x, -1);
                    glVertex2f(x, 1);
                }
                for(double y = -1.0; y < 1.0; y += window_style.grid_period) {
                    glVertex2f(-1, y);
                    glVertex2f(1, y);
                }
            }
            // рисуем график
            for(size_t nl = 0; nl < line_style.size(); ++nl) {
                glColor3f(line_style[nl].r, line_style[nl].g, line_style[nl].b);
                for(size_t i = 0; i < raw_data_y[nl].size() - 1; ++i) {
                    glVertex2f(get_x(raw_data_x[nl][i]), get_y(raw_data_y[nl][i]));
                    glVertex2f(get_x(raw_data_x[nl][i + 1]), get_y(raw_data_y[nl][i + 1]));
                }
            }
            // рисуем рамку
            glColor3f(window_style.fr, window_style.fg, window_style.fb);
            glVertex2f(-1, 1);
            glVertex2f(1, 1);

            glVertex2f(1, 1);
            glVertex2f(1, -1);

            glVertex2f(1, -1);
            glVertex2f(-1, -1);

            glVertex2f(-1, -1);
            glVertex2f(-1, 1);

            glEnd();
            glFlush();
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
                glutMainLoopEvent();
                return;
            }
            // необходима инициализация окна
            glutInitWindowSize(
                    window_style.width,
                    window_style.height);
            glutInitWindowPosition(0, 0);
            win_id = glutCreateWindow(window_name.c_str());
            glutSetWindow(win_id);

            glClearColor(window_style.br, window_style.bg, window_style.bb, 1.0);
            glMatrixMode(GL_PROJECTION); /*Настроим 2-х мерный вид*/
            glLoadIdentity();

            glOrtho(
                    -1.0 - window_style.indent, 1.0  + window_style.indent,
                    -1.0 - window_style.indent, 1.0  + window_style.indent,
                    -1.0, 1.0);
            ::glutDisplayFunc(update);
            glutMainLoopEvent();
            is_window_init = true;
        }
    };
//------------------------------------------------------------------------------
    static std::mutex drawings_mutex;
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
    /** \brief Cтроит несколько графиков, используя одинаковые оси для всех линий
     * \param name имя окна
     * \param wstyle стиль окна
     * \param count количество векторов и стилей
     * \param ... перечисление векторов и стилей по порядку (y2, style2, y3, style2 и т.д.)
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1>
    int plot(std::string name, WindowSpec wstyle, int count, ...) {
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
            drawings[pos]->init(name, wstyle, data, line_style);
        } else {
            drawings.push_back(new Drawing(name, wstyle, data, line_style));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
    }
//------------------------------------------------------------------------------
    /** \brief Cтроит несколько графиков, используя одинаковые оси для всех линий
     * \param name имя окна
     * \param count количество векторов и стилей
     * \param ... перечисление векторов и стилей по порядку (y2, style2, y3, style2 и т.д.)
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1>
    int plot(std::string name, int count, ...) {
        va_list args;
        va_start(args, count);
        int err = plot<T1>(name, WindowSpec(), count, args);
        va_end(args);
        return err;
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
            drawings[pos]->init(name, WindowSpec(), x, y, style);
        } else {
            drawings.push_back(new Drawing(name, WindowSpec(), x, y, style));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
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
            drawings[pos]->init(name, WindowSpec(), y, style);
        } else {
            drawings.push_back(new Drawing(name, WindowSpec(), y, style));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
    }
//------------------------------------------------------------------------------
}

#endif // EASY_PLOT_HPP_INCLUDED
