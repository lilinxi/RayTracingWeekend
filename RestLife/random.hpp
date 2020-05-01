#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>

struct RandomConfig {
    std::default_random_engine RandomEngine = std::default_random_engine(time(nullptr));
    double RandomScale = 1000.0; // 使小数随机更加精确
} RandomConfigInstance;

class Random {
public:
//        double 下 p(end) = 0 因此并不关心
    static double GenUniformRandom(double start, double end) {
        std::uniform_int_distribution<double> dis(start * RandomConfigInstance.RandomScale,
                                                  end * RandomConfigInstance.RandomScale);
        return dis(RandomConfigInstance.RandomEngine) / RandomConfigInstance.RandomScale;
    }

//        end 不可取值
    static int GenUniformRandomi(int start, int end) {
        std::uniform_int_distribution<int> dis(start, end - 1);
        return dis(RandomConfigInstance.RandomEngine);
    }

    static double GenNormalRandom(double mean, double stddev) {
        std::normal_distribution<double> dis(mean * RandomConfigInstance.RandomScale, stddev);
        return dis(RandomConfigInstance.RandomEngine) / RandomConfigInstance.RandomScale;
    }

    static bool GenBool() {
        return GenUniformRandom(0.0, 1.0) > 0.5;
    }
};

#endif //RANDOM_HPP
