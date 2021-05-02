#ifndef EASY_PLOT_HSV_TO_RGB_HPP_INCLUDED
#define EASY_PLOT_HSV_TO_RGB_HPP_INCLUDED

namespace easy_plot {
    namespace utility {

        /** \brief Преобразовать значение HSV в RGB
         * Данная функция нужна чтобы нарисовать тепловую карту
         * Функция принимает на входе значения от 0 до 1
         * \param value
         * \param r красный цвет
         * \param g зеленый цвет
         * \param b синий цвет
         */
        static void hsv_to_rgb(const float &value, float &r, float &g, float &b) {
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
    }; // utility
}; // easy_plot

#endif // EASY_PLOT_HSV_TO_RGB_HPP_INCLUDED
