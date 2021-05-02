#ifndef EASY_COMMON_HPP_INCLUDED
#define EASY_COMMON_HPP_INCLUDED

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

    static inline int EASY_PLOT_DEF_WIDTH            = 480; /**< Ширина по умолчанию */
    static inline int EASY_PLOT_DEF_HEIGHT           = 240; /**< Высота по умолчанию */
    static inline const double EASY_PLOT_DEF_INDENT  = 0.1; /**< Размер отсутпа от границы окна */

    enum TypesErrors {
        EASY_PLOT_OK = 0,               ///< Ошибок нет
        EASY_PLOT_INVALID_PARAMETR = 1, ///< Неверно указан параметр
    };

};

#endif // EASY_COMMON_HPP_INCLUDED
