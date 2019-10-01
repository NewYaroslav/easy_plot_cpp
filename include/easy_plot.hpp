#ifndef EASY_PLOT_HPP_INCLUDED
#define EASY_PLOT_HPP_INCLUDED

#include "GL/freeglut.h"
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <stdarg.h>
#include <numeric>

namespace easy_plot {
//------------------------------------------------------------------------------
    static const int EASY_PLOT_DEF_WIDTH       = 480;  /**< Ширина по умолчанию */
    static const int EASY_PLOT_DEF_HEIGHT      = 240;  /**< Высота по умолчанию */
    static const double EASY_PLOT_DEF_INDENT   = 0.1;  /**< Размер отсутпа от границы окна */

    static GLubyte *pixels = NULL;                     /**< Необходимо для создания файлов изображений */
//------------------------------------------------------------------------------
    enum TypesErrors {
        EASY_PLOT_OK = 0,               ///< Ошибок нет
        EASY_PLOT_INVALID_PARAMETR = 1, ///< Неверно указан параметр
    };
//------------------------------------------------------------------------------
    /** \brief Класс нужен для очистки памяти pixels
     */
    class MainPainter {
    public:
        ~MainPainter() {
            if(pixels != NULL) {
                free(pixels);
            }
        }
    };
//------------------------------------------------------------------------------
    static MainPainter painter;
//------------------------------------------------------------------------------
    /** \brief Преобразовать значение HSV в RGB
     * Данная функция нужна чтобы нарисовать тепловую карту
     * Функция принимает на входе значения от 0 до 1
     * \param value
     * \param r красный цвет
     * \param g зеленый цвет
     * \param b синий цвет
     */
    void hsv_to_rgb(const float &value, float &r, float &g, float &b) {
        uint32_t H = (uint32_t)(value * 240.0); // используем не полный цветовой круг
        uint32_t Hi = (uint32_t)(H / 60) % 6;
        float a = (float)(H % 60) / 60.0;
        switch(Hi) {
        case 0:
            r = 1.0;
            g = a;
            b = 0.0;
            break;
        case 1:
            r = 1.0 - a;
            g = 1.0;
            b = 0.0;
            break;
        case 2:
            r = 0.0;
            g = 1.0;
            b = a;
            break;
        case 3:
            r = 0.0;
            g = 1.0 - a;
            b = 1.0;
            break;
        case 4:
            r = a;
            g = 0.0;
            b = 1.0;
            break;
        case 5:
            r = 1.0;
            g = 0.0;
            b = 1.0 - a;
            break;
        }
    }
//------------------------------------------------------------------------------
    /** \brief Класс для хранения данных стиля окна
     */
    class WindowSpec {
        public:
        uint32_t width = EASY_PLOT_DEF_WIDTH;       /**< Шириная окна графика */
        uint32_t height = EASY_PLOT_DEF_HEIGHT;     /**< Высота окна графика */
        double indent = EASY_PLOT_DEF_INDENT;       /**< Отступ от края окна */
        double indent_frame = EASY_PLOT_DEF_INDENT; /**< Отступ от края рамки */

        double ir = 1.0;    /** цвет изображения, уровень красного */
        double ig = 1.0;    /** цвет изображения, уровень зеленого */
        double ib = 1.0;    /** цвет изображения, уровень синего */

        double br = 0.0;    /** цвет фона, уровень красного */
        double bg = 0.0;    /** цвет фона, уровень зеленого */
        double bb = 0.0;    /** цвет фона, уровень синего */

        //@{
        /** цвет сетки (R G B) */
        double gr = 0.1, gg = 0.1, gb = 0.1;
        //@}

        //@{
        /** цвет рамки (R G B) */
        double fr = 1.0, fg = 1.0, fb = 1.0;
        //@}

        //@{
        /** цвет текста (R G B) */
        double tr = 1.0, tg = 1.0, tb = 1.0;
        //@}

        //@{
        /** цвет указателя мыши (R G B) */
        double mr = 0.5, mg = 0.5, mb = 0.5;
        //@}
        bool is_zero_x_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_zero_y_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_grid = true;                    /**< Использовать сетку */
        bool is_color_heatmap = true;           /**< Использовать цветную тепловую карту вместо черно-белой */
        double grid_period = 0.1;               /**< Период сетки */
        void *font = GLUT_BITMAP_8_BY_13;       /**< Шрифт текста */

        /** \brief Инициализация параметров Окна по умолчанию
         */
        WindowSpec() { };
    };

    static WindowSpec default_window_style; /**< Стиль окна по умолчанию */
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
    static std::vector<std::shared_ptr<Drawing>> drawings;  /**< Список окон для отрисовки граиков */
    static int window_screen_x = 0;                         /**< Положение окон по умолчанию */
    static int window_screen_y = 0;                         /**< Положение окон по умолчанию */
    static int windiw_entry_id = 0;                         /**< ID окна, в котором находится пользователь */
//------------------------------------------------------------------------------
    /** \brief Класс для отрисовки графиков
     */
    class Drawing {
        private:
        WindowSpec window_style;                        /**< Стиль окна */
        std::vector<std::vector<double>> raw_data_y;    /**< Данные для рисования всех графиков */
        std::vector<std::vector<double>> raw_data_x;    /**< Данные для рисования всех графиков */
        std::vector<LineSpec> line_style;               /**< Стили линий для всех графиков */

        // для отображения изображений
        std::unique_ptr<float[]> raw_image_data;        /**< Сырые данные изображения */
        std::unique_ptr<float[]> scale_image_data;      /**< Масштабированные данные изображени */
        size_t image_width = 0;                         /**< Ширина изображения */
        size_t image_height = 0;                        /**< Высота изображения */
        bool is_draw_image = false;                     /**< Флаг отрисовки изображения */

        //@{
        /** Минимумы и максимумы данных для рисования графиков */
        double min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0;
        //@}
        bool is_window_init = false;                    /**< Флаг инициализации окна */
        bool is_raw_data = false;                       /**< Флаг инициализации данных графика */

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
        std::vector<std::vector<int>> get_x_axis_values_for_size(const size_t size) {
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
        std::vector<std::vector<int>> get_x_axis_values(const std::vector<std::vector<T1>> & data) {
            std::vector<std::vector<int>> temp(data.size());
            for(size_t  i = 0; i < data.size(); ++i) {
                temp[i].resize(data[i].size());
                std::iota(std::begin(temp[i]), std::end(temp[i]), 0);

            }
            return temp;
        }

        template<typename T1>
        std::vector<std::vector<T1>> get_axis_values(const std::vector<T1> &data) {
            std::vector<std::vector<T1>> temp;
            temp.push_back(data);
            return temp;
        }

        template<typename T1>
        std::vector<T1> get_style_values(const T1 &data) {
            std::vector<T1> temp;
            temp.push_back(data);
            return temp;
        }

        /** \brief Напечатать текст
         * \param x позиция в окне по оси X
         * \param y позиция в окне по оси Y
         * \param spacing дополнителный пробел
         * \param font шрифт
         * \param str строка
         */
        void render_spaced_bitmap_string(
                                    float x,
                                    float y,
                                    float spacing,
                                    void *font,
                                    const std::string &str) {
            glRasterPos2f(x, y);
            for(size_t i = 0; i < str.size(); ++i) {
                glutBitmapCharacter(font, (int)str[i]);
                x = x + glutBitmapWidth(font, (int)str[i]) + spacing;
            }
        }

        /** \brief Преобразовать пиксели в относительное расстояние
         * Данная функция нужна для отрисовки текста возле курсора мыши
         * \param value Значение в пикселях
         * \param size Максимальное количество пикселей
         * \return относительное расстояние
         */
        double convert_pixel_to_relative_len(int value, size_t size) {
            const double max_len = 2.0 + window_style.indent * 2.0;
            return ((double)value / (double) size) * max_len;
        }

        /** \brief Преобразовать пиксели в относительное расстояние
         * Данная функция нужна для получения положения курсора мыши
         * \param value Значение в пикселях
         * \param size Максимальное количество пикселей
         * \return относительное расстояние
         */
        double convert_pixel_to_relative(int value, size_t size) {
            const double max_len = 2.0 + window_style.indent * 2.0;
            return ((double)value / (double) size) * max_len - max_len / 2.0;
        }

        public:
        static Drawing* current_instance;       /**< Указатель на класс */
        std::string window_name;                /**< Имя окна графика */
        std::string save_image_name;            /**< Имя файла для сохранения изображения */

        int win_id = -1;    /**< Униклаьный ID графика */
        int width = 0;      /** Актуальная ширина окна */
        int height = 0;     /** Актуальная высота окна */

        double mouse_x = 0.0;   /** Позиция мыши по оси X */
        double mouse_y = 0.0;   /** Позиция мыши по оси Y */
        bool is_use_mouse = false;  /**< Флаг использования мыши в окне */
        bool is_save_image = false; /**< Флаг сохранения изображения */

        /** \brief Установить позицию мыши
         * \param x координаты по оси X
         * \param y координаты по оси Y
         */
        void set_mouse_position(int x, int y) {
            if(width == 0 || height == 0) return;
            mouse_x = convert_pixel_to_relative(x, width);
            mouse_y = convert_pixel_to_relative(height - y, height);
        }

        /** \brief Закрыть окно
         * Данная функция нужна для внутреннего использования
         */
        void close() {
            is_window_init = false;
            is_raw_data = false;
        }

        Drawing() { };

        /** \brief Инициализация графика
         * \param name имя окна
         * \param wstyle парамтеры стиля окна
         * \param x значения графика X
         * \param y значения графика Y
         * \param styles стили линий
         */
        template <typename T1, typename T2>
        void init(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &x, const std::vector<std::vector<T2>> &y, const std::vector<LineSpec> &styles) {
            is_raw_data = false;
            is_draw_image = false;
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
            if(wstyle.is_zero_x_line && min_y >= 0.0) min_y = 0.0;
            // изменяем min и max по оси Y чтобы был отступ
            double additive = window_style.indent_frame * (max_y - min_y);
            max_y += additive;
            min_y -= additive;

            is_raw_data = true;
            if(is_window_init) glutPostWindowRedisplay(win_id);
            //if(win_id == windiw_entry_id) glutPostRedisplay();
        }

        /** \brief Инициализация графика
         * \param name имя окна
         * \param wstyle стиль окна
         * \param y вектор по оси Y
         * \param style стиль линии
         */
        template <typename T1>
        void init(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style) {
            std::vector<LineSpec> vstyle = get_style_values(style);
            std::vector<std::vector<int>> vx = get_x_axis_values_for_size(y.size());
            std::vector<std::vector<T1>> vy = get_axis_values(y);
            init(name, wstyle, vx, vy, vstyle);
        }

        template <typename T1>
        Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style) {
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
        void init(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style) {
            std::vector<LineSpec> vstyle = get_style_values(style);
            std::vector<std::vector<T1>> vx = get_axis_values(x);
            std::vector<std::vector<T2>> vy = get_axis_values(y);
            init(name, wstyle, vx, vy, vstyle);
        }

        template <typename T1, typename T2>
        Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style) {
            init(name, wstyle, x, y, style);
        };

        /** \brief Инициализация графика
         * \param name имя окна
         * \param y
         * \param style стиль линии
         */
        template <typename T1>
        void init(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &vec, const std::vector<LineSpec> &styles) {
            std::vector<std::vector<int>> vx = get_x_axis_values(vec);
            init(name, wstyle, vx, vec, styles);
        }

        template <typename T1>
        Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &vec, const std::vector<LineSpec> &styles) {
            init(name, wstyle, vec, styles);
        };

        /** \brief Инициализируем тепловую карту
         * \param name имя окна
         * \param wstyle стиль окна
         * \param image_data указатель на массив данных
         * \param width ширина изображения
         * \param height высота изображения
         */
        void init(const std::string &name, const WindowSpec &wstyle, const float *image_data, const size_t width, const size_t height) {
            is_raw_data = false;
            is_draw_image = false;
            if(name == "" || width == 0 || height == 0) return;
            size_t old_size = image_width * image_height;
            size_t new_size = width * height;
            if(new_size > old_size) {
                raw_image_data = std::unique_ptr<float[]>(new float[new_size]);
                scale_image_data = std::unique_ptr<float[]>(new float[new_size]);
            }
            image_width = width;
            image_height = height;
            float *point_raw_image_data = raw_image_data.get();
            std::copy(image_data, image_data + new_size, point_raw_image_data);
            float min_data = std::numeric_limits<float>::max();
            float max_data = std::numeric_limits<float>::lowest();

            for(size_t i = 0; i < new_size; ++i) {
                if(max_data < point_raw_image_data[i]) max_data = point_raw_image_data[i];
                if(min_data > point_raw_image_data[i]) min_data = point_raw_image_data[i];
            } // for i

            float scale_factor = max_data == min_data ? 0.0 : 1.0 / (max_data - min_data);
            float *point_scale_image_data = scale_image_data.get();
            for(size_t i = 0; i < new_size; ++i) {
                point_scale_image_data[i] = (point_raw_image_data[i] - min_data) * scale_factor;
            } // for i

            window_name = name;
            window_style = wstyle;
            is_draw_image = true;
            is_raw_data = true;
            if(is_window_init) glutPostWindowRedisplay(win_id);
            //if(win_id == windiw_entry_id) glutPostRedisplay();
        }

        /** \brief Инициализируем изображение
         * \param name имя окна
         * \param wstyle стиль окна
         * \param image_data указатель на массив данных
         * \param width ширина изображения
         * \param height высота изображения
         */
        Drawing(const std::string &name, const WindowSpec &wstyle, const float *image_data, const size_t width, const size_t height) {
            init(name, wstyle, image_data, width, height);
        };

        /** \brief Функция рисования графика
         */
        void draw() {
            if(!is_raw_data) return; // если график не был инициализирован
            glClear(GL_COLOR_BUFFER_BIT);

            // рисуем сетку
            if(!is_draw_image) {
                glBegin(GL_LINES);
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
                glEnd();
            }

            // рисуем линию нуля по осям
            if(!is_draw_image) {
                glBegin(GL_LINES);
                // рисуем линию нуля по оси X
                if(window_style.is_zero_x_line) {
                    glColor3f(window_style.fr, window_style.fg, window_style.fb);
                    glVertex2f(-1, get_y(0));
                    glVertex2f(1, get_y(0));
                }
                // рисуем линию нуля по оси Y
                if(window_style.is_zero_y_line) {
                    glColor3f(window_style.fr, window_style.fg, window_style.fb);
                    glVertex2f(get_x(0), -1);
                    glVertex2f(get_x(0), 1);
                }
                glEnd();
            }

            // для поиска минимальной дистнации до курсора мыши
            double min_distance = 4.0 + 4.0 * window_style.indent;

            double real_mouse_x = 0.0, real_mouse_y = 0.0;
            double mouse_data_x = 0.0; // значение элемента массива по оси X
            double mouse_data_y = 0.0; // значение элемента массива по оси Y
            double mouse_image_data = 0.0; // значение пикселя изображения
            size_t mouse_image_x = 0;
            size_t mouse_image_y = 0;
            int indx_raw_data = 0;

            if(!is_draw_image) {
                glBegin(GL_LINES);
                // рисуем график
                for(size_t nl = 0; nl < line_style.size(); ++nl) {
                    glColor3f(line_style[nl].r, line_style[nl].g, line_style[nl].b);
                    for(size_t i = 0; i < raw_data_y[nl].size() - 1; ++i) {
                        double x1 = get_x(raw_data_x[nl][i]);
                        double x2 = get_x(raw_data_x[nl][i + 1]);
                        double y1 = get_y(raw_data_y[nl][i]);
                        double y2 = get_y(raw_data_y[nl][i + 1]);
                        if(is_use_mouse) {
                            double distance_xy1 = std::abs(x1 - mouse_x) + std::abs(y1 - mouse_y);
                            double distance_xy2 = std::abs(x2 - mouse_x) + std::abs(y2 - mouse_y);
                            if(distance_xy1 < min_distance) {
                                mouse_data_x = raw_data_x[nl][i];
                                mouse_data_y = raw_data_y[nl][i];
                                indx_raw_data = nl;
                                real_mouse_x = x1;
                                real_mouse_y = y1;
                                min_distance = distance_xy1;
                            }
                            if(distance_xy2 < min_distance) {
                                mouse_data_x = raw_data_x[nl][i + 1];
                                mouse_data_y = raw_data_y[nl][i + 1];
                                indx_raw_data = nl;
                                real_mouse_x = x2;
                                real_mouse_y = y2;
                                min_distance = distance_xy2;
                            }
                        }
                        glVertex2f(x1, y1);
                        glVertex2f(x2, y2);
                    }
                }
                glEnd();
            } else {
                // рисуем изображение
                glBegin(GL_QUADS);
                glColor3f(window_style.ir, window_style.ig, window_style.ib);
                double step_x = 2.0/(double)image_width;
                double step_y = 2.0/(double)image_height;
                size_t image_ind = 0;
                for(size_t x = 0; x < image_width; ++x) {
                    double x1 = -1.0 + (double)x * step_x;
                    double x2 = x1 + step_x;
                    for(size_t y = 0; y < image_height; ++y, ++image_ind) {
                        double y2 = 1.0 - (double)y * step_y;
                        double y1 = y2 - step_y;

                        if(!window_style.is_color_heatmap) {
                            glColor3f(
                                window_style.ir * scale_image_data[image_ind],
                                window_style.ig * scale_image_data[image_ind],
                                window_style.ib * scale_image_data[image_ind]);
                        } else {
                            float r, g, b;
                            hsv_to_rgb(scale_image_data[image_ind], r, g, b);
                            glColor3f(r,g,b);
                        }
                        glVertex2f(x1, y2);
                        glVertex2f(x1, y1);
                        glVertex2f(x2, y1);
                        glVertex2f(x2, y2);
                    }
                }
                glEnd();

                if(is_use_mouse) {
                    mouse_image_x = (mouse_x + 1.0)/step_x;
                    mouse_image_y = (1.0 - mouse_y)/step_y;
                    double x1 = -1.0 + (double)mouse_image_x * step_x;
                    double x2 = x1 + step_x;
                    double y2 = 1.0 - (double)mouse_image_y * step_y;
                    double y1 = y2 - step_y;
                    real_mouse_x = (x1 + x2) / 2.0;
                    real_mouse_y = (y1 + y2) / 2.0;
                    mouse_image_data = raw_image_data[mouse_image_x * image_height + mouse_image_y];
                }
            }

            // рисуем сетку для изображения
            if(is_draw_image && window_style.is_grid) {
                glBegin(GL_LINES);
                glColor3f(window_style.gr, window_style.gg, window_style.gb);
                double step_x = 2.0/(double)image_width;
                for(double x = -1.0; x < 1.0; x += step_x) {
                    glVertex2f(x, -1);
                    glVertex2f(x, 1);
                }
                double step_y = 2.0/(double)image_height;
                for(double y = -1.0; y < 1.0; y += step_y) {
                    glVertex2f(-1, y);
                    glVertex2f(1, y);
                }
                glEnd();
            }

            // рисуем мышь
            if(is_use_mouse) {
                glBegin(GL_LINES);
                glColor3f(window_style.mr, window_style.mg, window_style.mb);
                glVertex2f(-1, real_mouse_y);
                glVertex2f(1, real_mouse_y);
                glVertex2f(real_mouse_x, -1);
                glVertex2f(real_mouse_x, 1);
                glEnd();
            }

            // рисуем рамку
            glBegin(GL_LINES);
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

            // если естьмышь, отобразим линию
            if(is_use_mouse) {
                if(!is_draw_image) {
                    std::string text_line("line ");
                    text_line += std::to_string(indx_raw_data);
                    std::string text_x(std::to_string(mouse_data_x));
                    std::string text_y(std::to_string(mouse_data_y));

                    glColor3f(window_style.tr, window_style.tg, window_style.tb);
                    const double dh = convert_pixel_to_relative_len(glutBitmapHeight(window_style.font), height);
                    const double OFFSET_Y = 0.01;

                    render_spaced_bitmap_string(real_mouse_x, real_mouse_y + dh * 2 + OFFSET_Y, 0.0, window_style.font, text_line);
                    render_spaced_bitmap_string(real_mouse_x, real_mouse_y + dh + OFFSET_Y, 0.0, window_style.font, text_x);
                    render_spaced_bitmap_string(real_mouse_x, real_mouse_y + OFFSET_Y, 0.0, window_style.font, text_y);
                } else {
                    std::string text_line("pixel ");
                    text_line += std::to_string(mouse_image_x);
                    text_line += " ";
                    text_line += std::to_string(mouse_image_y);
                    std::string text_pixel(std::to_string(mouse_image_data));

                    glColor3f(window_style.tr, window_style.tg, window_style.tb);
                    const double dh = convert_pixel_to_relative_len(glutBitmapHeight(window_style.font), height);
                    const double OFFSET_Y = 0.02;

                    const double dw = convert_pixel_to_relative_len(glutBitmapLength(window_style.font, (const unsigned char*)text_line.c_str()), width);
                    const double OFFSET_X = 0.02;

                    // реализуем правильное размещение текста на экране в зависимости от расположения курсора
                    double string_x = real_mouse_x;
                    double string_y = real_mouse_y;
                    if((1.0 - real_mouse_x - OFFSET_X) < dw) {
                        string_x = real_mouse_x - dw - OFFSET_X;
                    }
                    if((1.0 - real_mouse_y - OFFSET_Y) < dh * 2) {
                        string_y = real_mouse_y - dh * 2 - OFFSET_Y;
                    }
                    render_spaced_bitmap_string(string_x, string_y + dh * 1 + OFFSET_Y, 0.0, window_style.font, text_line);
                    render_spaced_bitmap_string(string_x, string_y + dh * 0 + OFFSET_Y, 0.0, window_style.font, text_pixel);
                }
            }

            glFlush();

            if(is_save_image) {
                screenshot_ppm(save_image_name.c_str(), width, height, &pixels);
                is_save_image = false;
            }
        }

        /** \brief Обработчик перерисовки экрана
         */
        static void update_draw() {
            int win_id = glutGetWindow();
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->draw();
                    break;
                }
            }
        }

        /** \brief Событие закрытия окна
         */
        static void event_closing() {
            int win_id = glutGetWindow();
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->close();
                    drawings.erase(drawings.begin() + i);
                    break;
                }
            }
        }

        /** \brief Событие движения мыши внутри окна
         */
        static void event_mouse_move(int x, int y) {
            int win_id = glutGetWindow();
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->set_mouse_position(x, y);
                    glutPostRedisplay();
                    break;
                }
            }
        }

        /** \brief Событие входа или выхода мыши из текущего окна
         */
        static void event_entry(int state) {
            int win_id = glutGetWindow();
            if(state == GLUT_ENTERED) windiw_entry_id = win_id;
            else windiw_entry_id = -1;
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->is_use_mouse = state == GLUT_ENTERED ? true : false;
                    glutPostRedisplay();
                    break;
                }
            }
        }

        /** \brief Обработчик изменения размера окна
         */
        static void event_reshape(int width, int height) {
            int win_id = glutGetWindow();
            for(size_t i = 0; i < drawings.size(); ++i) {
                if(drawings[i]->win_id == win_id) {
                    drawings[i]->width = width;
                    drawings[i]->height = height;
                    glViewport(0, 0, width, height);
                    break;
                }
            }
        }

        /** \brief Обновить состояние окна
         */
        void update_window() {
            if(is_save_image) glutPostRedisplay();
            if(is_window_init) {
                glutMainLoopEvent();
                return;
            }
            // необходима инициализация окна
            glutInitWindowSize(
                    window_style.width,
                    window_style.height);
            // узнаем размеры экрана
            int screen_w = glutGet(GLUT_SCREEN_WIDTH);
            int screen_h = glutGet(GLUT_SCREEN_HEIGHT);
            const int OFFSET_W = 16;
            const int OFFSET_H = 44;
            // проверяем, вписываемся ли мы в монитор
            if((window_screen_x + (int)window_style.width + OFFSET_W) > screen_w) {
                window_screen_x = 0;
                window_screen_y += ((int)window_style.height + OFFSET_H);
                if((window_screen_y + (int)window_style.height + OFFSET_H) > screen_h) {
                    window_screen_y = 0;
                }
            }
            glutInitWindowPosition(window_screen_x, window_screen_y);
            window_screen_x += (window_style.width + OFFSET_W);

            win_id = glutCreateWindow(window_name.c_str());
            glutSetWindow(win_id);

            glClearColor(window_style.br, window_style.bg, window_style.bb, 1.0);
            glMatrixMode(GL_PROJECTION); /*Настроим 2-х мерный вид*/
            glLoadIdentity();

            glOrtho(
                    -1.0 - window_style.indent, 1.0  + window_style.indent,
                    -1.0 - window_style.indent, 1.0  + window_style.indent,
                    -1.0, 1.0);
            ::glutDisplayFunc(update_draw);
            ::glutCloseFunc(event_closing);
            ::glutReshapeFunc(event_reshape);
            ::glutPassiveMotionFunc(event_mouse_move);
            ::glutEntryFunc(event_entry);
            glutMainLoopEvent();
            is_window_init = true; // ставим флаг инициализации окна
        }

        static void screenshot_ppm(
            const char *filename,
            unsigned int width,
            unsigned int height,
            GLubyte **pixels) {
                size_t i, j, cur;
                const size_t format_nchannels = 3;
                FILE *f = fopen(filename, "w");
                fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
                *pixels = (GLubyte*)realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height);
                glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *pixels);
                for (i = 0; i < height; i++) {
                    for (j = 0; j < width; j++) {
                        cur = format_nchannels * ((height - i - 1) * width + j);
                        fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
                    }
                    fprintf(f, "\n");
                }
                fclose(f);
        }
    };
//------------------------------------------------------------------------------
    static std::mutex drawings_mutex;           /**< Мьютекс для работы с данными изображений */
    static std::future<void> drawings_future;   /**< Для асинхронного потока отрисовки изображений */
//------------------------------------------------------------------------------
    /** \brief Инициализировать работу с графиками
     * \param argc аргумент командной строки
     * \param argv аргумент командной строки
     */
    void init(int *argc, char *argv[]) {
        drawings_future = std::async(std::launch::async,[&, argc, argv]() {
        //std::thread glut_thread([&, argc, argv]() {
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
        //glut_thread.detach();
    }
//------------------------------------------------------------------------------
    int get_pos_plot(const std::string &name) {
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
    int plot(const std::string &name, const WindowSpec &wstyle, const size_t count, ...) {
        va_list va;
        va_start(va, count);

        std::vector<std::vector<T1>> data;
        std::vector<LineSpec> line_style;
        for(size_t n = 0; n < count; ++n) {
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
            drawings.push_back(std::make_shared<Drawing>(name, wstyle, data, line_style));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
    }
//------------------------------------------------------------------------------
    /** \brief Создает двухмерный линейный график данных в Y по сравнению с соответствующими значениями в X.
     * Функция графика отображает Y против X,
     * X и Y должны иметь одинаковую длину.
     * \param name имя окна
     * \param wstyle стиль окна
     * \param x вектор по оси X
     * \param y вектор по оси Y
     * \param style стиль линии
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1, typename T2>
    int plot(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style = LineSpec()) {
        if(x.size() != y.size()) return EASY_PLOT_INVALID_PARAMETR;
        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, wstyle, x, y, style);
        } else {
            drawings.push_back(std::make_shared<Drawing>(name, wstyle, x, y, style));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
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
    int plot(const std::string &name, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style = LineSpec()) {
        return plot(name, default_window_style, x, y, style);
    }
//------------------------------------------------------------------------------
    /** \brief Создает двухмерный линейный график данных в Y по сравнению с индексом каждого значения.
     * \param name имя окна
     * \param wstyle стиль окна
     * \param y вектор по оси Y
     * \param style стиль линии
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    template <typename T1>
    int plot(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style = LineSpec()) {
        if(y.size() <= 1) return EASY_PLOT_INVALID_PARAMETR;
        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, wstyle, y, style);
        } else {
            drawings.push_back(std::make_shared<Drawing>(name, wstyle, y, style));
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
    int plot(const std::string &name, const std::vector<T1> &y, const LineSpec &style = LineSpec()) {
        return plot(name, default_window_style, y, style);
    }
//------------------------------------------------------------------------------
    /** \brief Сохранить изображение
     * \param window_name имя окна, которое необходимо сохранить
     * \param file_name имя файла изображения
     */
    void save_image(std::string window_name, std::string file_name) {
        drawings_mutex.lock();
        int pos = get_pos_plot(window_name);
        if(pos >= 0) {
            drawings[pos]->save_image_name = file_name;
            drawings[pos]->is_save_image = true;
        }
        drawings_mutex.unlock();
    }
//------------------------------------------------------------------------------
    /** \brief Нарисовать тепловую карту
     * Для тепловой карты точка 0:0 находится в левом верхнем углу.
     * Функция рисования проходит массив тепловой карты сначала по строкам, затем по столбцам
     * Поэтому на вход функции можно подать двумерный массив
     * \param name Имя тепловой карты
     * \param wstyle Настройки окна
     * \param image Указатель на массив данных
     * \param width Ширина тепловой карты
     * \param height Высота тепловой карты
     * \return состояние ошибки, 0 в случае успеха, иначе см. TypesErrors
     */
    int draw_heatmap(const std::string &name, const WindowSpec &wstyle, const float* image, const size_t width, const size_t height) {
        if(width == 0 || height == 0 || name.size() == 0) return EASY_PLOT_INVALID_PARAMETR;
        drawings_mutex.lock();
        int pos = get_pos_plot(name);
        if(pos >= 0) {
            drawings[pos]->init(name, wstyle, image, width, height);
        } else {
            drawings.push_back(std::make_shared<Drawing>(name, wstyle, image, width, height));
        }
        drawings_mutex.unlock();
        return EASY_PLOT_OK;
    }
//------------------------------------------------------------------------------
}

#endif // EASY_PLOT_HPP_INCLUDED
