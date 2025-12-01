#ifndef IMAGE_SEGMENTATION_H
#define IMAGE_SEGMENTATION_H

#include <string>

enum class Strategy {
    KRUSKAL_MST,
    EDMONDS_MSA,
    TARJAN_MSA,
    GABOW_MSA
};

struct PreprocessingOptions {
    bool enableBlur = true;     
    double minSuperpixelSize = 15.0; 
};

class ImageSegmentation {
public:
    static void runSegmentation(
        const std::string& inputPath, 
        const std::string& outputPath, 
        Strategy strategy, 
        double threshold,
        PreprocessingOptions options 
    );
};

#endif 