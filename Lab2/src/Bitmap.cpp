#include "Bitmap.hpp"
#include "ThreadPool.hpp"

static constexpr int THREADS_COUNT = 12;
static ThreadPool thread_pool{THREADS_COUNT};

Bitmap::Bitmap(int width, int height) noexcept 
    : m_data(width * height, 0)
    , m_width(width)
    , m_height(height)
    {}

void Bitmap::clear(){
    std::ranges::fill(m_data, 0);
}

const uint8_t* Bitmap::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Bitmap::draw_faces(const Vertices& points, 
                        const Faces& faces) 
{
    auto draw_line = [this](std::vector<uint32_t>& data, int x1, int y1, int x2, int y2) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            if (x1 >= 0 && x1 < m_width && y1 >= 0 && y1 < m_height) {
                int index = y1 * m_width + x1;
                data[index] = WHITE;
            }
            else break;

            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    };
    
    auto draw_partial = [&](auto it_begin, auto it_end){
        std::ranges::for_each(it_begin, it_end, [&](const Face& face){
            const size_t face_size = face.size();

            for (size_t i = 0; i < face_size; ++i) {
                const auto& point1 = points[face[i]];
                const auto& point2 = points[face[(i + 1) % face_size]];

                if (point1.w >= 0.999 && point2.w >= 0.999){
                    int x1 = static_cast<int>(point1.x);
                    int y1 = static_cast<int>(point1.y);
                    int x2 = static_cast<int>(point2.x);
                    int y2 = static_cast<int>(point2.y);
    
                    draw_line(m_data, x1, y1, x2, y2);
                }
            }
        });
    };

    clear();

    std::vector<std::future<std::any>> futures{};

    const int size = faces.size();
    int count_per_thread = size / THREADS_COUNT;
    
    for (int i = 0; i < THREADS_COUNT; ++i) {
        int current_size = (i < THREADS_COUNT - 1)
        ?   count_per_thread
        :   size - (THREADS_COUNT - 1) * count_per_thread;
        
        auto it_begin = std::next(faces.begin(), i * count_per_thread);
        auto it_end = std::next(it_begin, current_size);
        
        futures.emplace_back(thread_pool.add_task(
            draw_partial, it_begin, it_end
        ));
    }
    
    std::ranges::for_each(futures, [](auto& current_future){
        auto fut = current_future.get();
    });
}