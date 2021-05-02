#ifndef EASY_PLOT_SCRENNSHOT_HPP_INCLUDED
#define EASY_PLOT_SCRENNSHOT_HPP_INCLUDED

namespace easy_plot {
    namespace utility {

        /** \brief Класс для создания скриншотов
         */
        class Screenshot {
        private:
            GLubyte *pixels = nullptr;  /**< Необходимо для создания файлов изображений */

            void screenshot_ppm(
                    const char *filename,
                    const uint32_t width,
                    const uint32_t height,
                    GLubyte **img_pixels) noexcept {
                size_t i, j, cur;
                const size_t format_nchannels = 3;
                FILE *f = fopen(filename, "w");
                fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
                *img_pixels = (GLubyte*)realloc(*img_pixels, format_nchannels * sizeof(GLubyte) * width * height);
                glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *img_pixels);
                for (i = 0; i < height; i++) {
                    for (j = 0; j < width; j++) {
                        cur = format_nchannels * ((height - i - 1) * width + j);
                        fprintf(f, "%3d %3d %3d ", (*img_pixels)[cur], (*img_pixels)[cur + 1], (*img_pixels)[cur + 2]);
                    }
                    fprintf(f, "\n");
                }
                fclose(f);
            }
        public:

            enum class TypesFormats {
                USE_PPM,
            };

            Screenshot() {};

            ~Screenshot() {
                if(pixels != nullptr) {
                    free(pixels);
                }
            };

            void print_screen(const std::string &filename, const size_t width, const size_t height, const TypesFormats type) noexcept {
                switch(type) {
                case TypesFormats::USE_PPM:
                    screenshot_ppm(filename.c_str(), (uint32_t)width, (uint32_t)height, &pixels);
                    break;
                default:
                    break;
                };
            }
        };
    }
};

#endif // EASY_PLOT_SCRENNSHOT_HPP_INCLUDED
