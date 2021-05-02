#ifndef EASY_PLOT_WINDOW_SPEC_HPP_INCLUDED
#define EASY_PLOT_WINDOW_SPEC_HPP_INCLUDED

namespace easy_plot {

    /** \brief Класс для хранения данных стиля окна
     */
    class WindowSpec {
        public:
        uint32_t width = EASY_PLOT_DEF_WIDTH;       /**< Шириная окна графика */
        uint32_t height = EASY_PLOT_DEF_HEIGHT;     /**< Высота окна графика */
        double indent = EASY_PLOT_DEF_INDENT;       /**< Отступ от края окна */
        double indent_frame = EASY_PLOT_DEF_INDENT; /**< Отступ от края рамки */

        // цвет изображения
        double ir = 1.0;    /** цвет изображения, уровень красного */
        double ig = 1.0;    /** цвет изображения, уровень зеленого */
        double ib = 1.0;    /** цвет изображения, уровень синего */

        // цвет фона
        double br = 0.0;    /** цвет фона, уровень красного */
        double bg = 0.0;    /** цвет фона, уровень зеленого */
        double bb = 0.0;    /** цвет фона, уровень синего */

        // цвет сетки
        double gr = 0.1;    /**< цвет сетки, уровень красного */
        double gg = 0.1;    /**< цвет сетки, уровень зеленого */
        double gb = 0.1;    /**< цвет сетки, уровень синего */

        // цвет рамки
        double fr = 1.0;    /**< цвет рамки, уровень красного */
        double fg = 1.0;    /**< цвет рамки, уровень зеленого */
        double fb = 1.0;    /**< цвет рамки, уровень синего */

        // цвет текста
        double tr = 1.0;    /**< цвет текста, уровень красного */
        double tg = 1.0;    /**< цвет текста, уровень зеленого */
        double tb = 1.0;    /**< цвет текста, уровень синего */

        // цвет указателя мыши
        double mr = 0.5;    /**< цвет указателя мыши, уровень красного*/
        double mg = 0.5;    /**< цвет указателя мыши, уровень зеленого */
        double mb = 0.5;    /**< цвет указателя мыши, уровень синего */

        bool is_zero_x_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_zero_y_line = false;            /**< Линия нуля, если true то линия рисуется */
        bool is_grid = true;                    /**< Использовать сетку */
        bool is_color_heatmap = true;           /**< Использовать цветную тепловую карту вместо черно-белой */
        bool is_invert_color_heatmap = false;   /**< Инвертировать цвет тепловой карты */
        double grid_period = 0.1;               /**< Период сетки */

        void *font = GLUT_BITMAP_8_BY_13;       /**< Шрифт текста */

        /** \brief Инициализация параметров Окна по умолчанию
         */
        WindowSpec() {};
    };

}; // easy_plot

#endif // EASY_PLOT_WINDOW_SPEC_HPP_INCLUDED
