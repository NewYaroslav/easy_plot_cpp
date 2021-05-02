#ifndef EASY_PLOT_HPP_INCLUDED
#define EASY_PLOT_HPP_INCLUDED

#include "main/easy_plot_common.hpp"
#include "main/easy_plot_line_spec.hpp"
#include "main/easy_plot_window_spec.hpp"
#include "main/easy_plot_draw.hpp"

namespace easy_plot {

    class EasyPlot {
    private:
        inline static WindowSpec default_window_style; /**< Стиль окна по умолчанию */

        static inline std::future<void> drawings_future;   /**< Для асинхронного потока отрисовки изображений */
        static inline std::mutex drawings_mutex;           /**< Мьютекс для работы с данными изображений */

        class DrawingThread {
        public:
            std::mutex drawings_mutex;          /**< Мьютекс для работы с данными изображений */
            std::future<void> drawings_future;   /**< Для асинхронного потока отрисовки изображений */
            std::atomic<bool> is_shutdown = ATOMIC_VAR_INIT(false);

            DrawingThread() {};

            void init(int *argc, char *argv[]) {
                drawings_future = std::async(std::launch::async,[&, argc, argv]() {
                    using namespace tools;
                    // инициализируем FREEGLUT
                    glutInit(argc, argv);
                    // чтобы можно было закрывать окна
                    glutSetOption(
                        GLUT_ACTION_ON_WINDOW_CLOSE,
                        GLUT_ACTION_CONTINUE_EXECUTION);
                    // инициализируем режим отображения
                    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
                    while(!is_shutdown) {
                        // инициализируем все окна
                        std::lock_guard<std::mutex> lock(drawings_mutex);
                        for(size_t i = 0; i < Drawing::drawings.size(); ++i) {
                            Drawing::drawings[i]->update_window();
                            std::this_thread::yield();
                        }
                        std::this_thread::yield();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
            };

            ~DrawingThread() {
                is_shutdown = true;
                if(drawings_future.valid()) {
                    try {
                        drawings_future.wait();
                        drawings_future.get();
                    }
                    catch(const std::exception &e) {
                        std::cerr << "Error: ~DrawingThread(), what: " << e.what() << std::endl;
                    }
                    catch(...) {
                        std::cerr << "Error: ~DrawingThread()" << std::endl;
                    }
                }
            };
        };

        static inline DrawingThread drawing_thread;

    public:
        using WindowSpec = easy_plot::WindowSpec;
        using LineSpec = easy_plot::LineSpec;
//------------------------------------------------------------------------------
        /** \brief Инициализировать работу с графиками
         * \param argc аргумент командной строки
         * \param argv аргумент командной строки
         */
        static void init(int *argc, char *argv[]) {
            static bool is_once = false;
            if(!is_once) drawing_thread.init(argc, argv);
            is_once = true;
        }
//------------------------------------------------------------------------------
        static int get_pos_plot(const std::string &name) {
            using namespace tools;
            for(size_t i = 0; i < Drawing::drawings.size(); ++i) {
                if(Drawing::drawings[i]->window_name == name) return i;
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
        static int plot(const std::string &name, const WindowSpec &wstyle, const size_t count, ...) {
            using namespace tools;
            va_list va;
            va_start(va, count);

            std::vector<std::vector<T1>> data;
            std::vector<LineSpec> line_style;
            for(size_t n = 0; n < count; ++n) {
                std::vector<T1> vec_next = va_arg(va, std::vector<T1>);
                LineSpec style_next = va_arg(va, LineSpec );
                data.push_back(vec_next);
                line_style.push_back(style_next);
            }
            va_end(va);

            {
                std::lock_guard<std::mutex> lock(drawings_mutex);
                int pos = get_pos_plot(name);
                if(pos >= 0) {
                    Drawing::drawings[pos]->init(name, wstyle, data, line_style);
                } else {
                    Drawing::drawings.push_back(std::make_shared<Drawing>(name, wstyle, data, line_style));
                }
            }
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
        static int plot(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style = LineSpec()) {
            using namespace tools;
            if(x.size() != y.size()) return EASY_PLOT_INVALID_PARAMETR;

            std::lock_guard<std::mutex> lock(drawings_mutex);
            int pos = get_pos_plot(name);
            if(pos >= 0) {
                Drawing::drawings[pos]->init(name, wstyle, x, y, style);
            } else {
                Drawing::drawings.push_back(std::make_shared<Drawing>(name, wstyle, x, y, style));
            }
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
        static int plot(const std::string &name, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style = LineSpec()) {
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
        static int plot(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style = LineSpec()) {
            using namespace tools;
            if(y.size() <= 1) return EASY_PLOT_INVALID_PARAMETR;
            std::lock_guard<std::mutex> lock(drawings_mutex);
            int pos = get_pos_plot(name);
            if(pos >= 0) {
                Drawing::drawings[pos]->init(name, wstyle, y, style);
            } else {
                Drawing::drawings.push_back(std::make_shared<Drawing>(name, wstyle, y, style));
            }
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
        static int plot(const std::string &name, const std::vector<T1> &y, const LineSpec &style = LineSpec()) {
            return plot(name, default_window_style, y, style);
        }
//------------------------------------------------------------------------------
        /** \brief Сохранить изображение
         * \param window_name имя окна, которое необходимо сохранить
         * \param file_name имя файла изображения
         */
        static void save_image(std::string window_name, std::string file_name) {
            using namespace tools;
            std::lock_guard<std::mutex> lock(drawings_mutex);
            int pos = get_pos_plot(window_name);
            if(pos >= 0) {
                Drawing::drawings[pos]->save_image_name = file_name;
                Drawing::drawings[pos]->is_save_image = true;
            }
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
        static int draw_heatmap(const std::string &name, const WindowSpec &wstyle, const float* image, const size_t width, const size_t height) {
            using namespace tools;
            if(width == 0 || height == 0 || name.size() == 0) return EASY_PLOT_INVALID_PARAMETR;
            std::lock_guard<std::mutex> lock(drawings_mutex);
            int pos = get_pos_plot(name);
            if(pos >= 0) {
                Drawing::drawings[pos]->init(name, wstyle, image, width, height);
            } else {
                Drawing::drawings.push_back(std::make_shared<Drawing>(name, wstyle, image, width, height));
            }
            return EASY_PLOT_OK;
        }
//------------------------------------------------------------------------------
    }; // EasyPlot
}; // easy_plot

using ep = easy_plot::EasyPlot;

#endif // EASY_PLOT_HPP_INCLUDED
