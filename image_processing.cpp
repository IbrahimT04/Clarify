#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

// Global variables
cv::Mat originalImage, resizedImage, displayedImage;
bool isGrayscale = false;
bool isDOG = false;
bool isEnhancedDOG = false;

// Threshold and tuning variable for enhanced DOG
const double threshold = 128.0;  // Example threshold for luminance
const double newVariable = 0.05; // Tuning parameter for tanh function

void applyFilters() {
    cv::Mat workingImage = resizedImage.clone();

    if (isGrayscale) {
        cv::cvtColor(workingImage, workingImage, cv::COLOR_BGR2GRAY);
    }

    if (isDOG && !isGrayscale) {
        cv::Mat temp1, temp2;
        cv::cvtColor(workingImage, temp1, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(temp1, temp1, cv::Size(5, 5), 20);
        cv::GaussianBlur(temp1, temp2, cv::Size(5, 5), 1);
        workingImage = 10 * (temp2 - temp1);
    }

    if (isEnhancedDOG && !isGrayscale) {
        cv::Mat temp1, temp2, diffImage;
        cv::cvtColor(workingImage, temp1, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(temp1, temp1, cv::Size(5, 5), 20);
        cv::GaussianBlur(temp1, temp2, cv::Size(5, 5), 1);
        diffImage = 100 * (temp2 - temp1);

        for (int y = 0; y < diffImage.rows; ++y) {
            for (int x = 0; x < diffImage.cols; ++x) {
                double value = diffImage.at<uchar>(y, x);
                if (value > threshold) {
                    diffImage.at<uchar>(y, x) = 255;  // White if above threshold
                } else {
                    diffImage.at<uchar>(y, x) = static_cast<uchar>(255 * (1 + tanh(newVariable * (value - threshold))) / 2);
                }
            }
        }
        workingImage = diffImage;
    }

    displayedImage = workingImage.clone();
    cv::imshow("Displayed Image", displayedImage);
}

void applyReset(int, void*) {
    displayedImage = resizedImage.clone();
    isGrayscale = isDOG = isEnhancedDOG = false;
    cv::imshow("Displayed Image", displayedImage);
}

void applyGrayscaleFilter(int, void*) {
    isGrayscale = !isGrayscale;
    applyFilters();
}

void applyBoxBlurFilter(int, void*) {
    cv::blur(displayedImage, displayedImage, cv::Size(5, 5));
    cv::imshow("Displayed Image", displayedImage);
}

void applyGaussianBlurFilter(int, void*) {
    cv::GaussianBlur(displayedImage, displayedImage, cv::Size(5, 5), 0);
    cv::imshow("Displayed Image", displayedImage);
}

void applyDifferenceOfGaussiansFilter(int, void*) {
    isDOG = !isDOG;
    applyFilters();
}

void applyEnhancedDifferenceOfGaussiansFilter(int, void*) {
    isEnhancedDOG = !isEnhancedDOG;
    applyFilters();
}

void applySharpeningFilter(int, void*) {
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        -0.5, -0.5, -0.5,
        -0.5, 4.5, -0.5,
        -0.5, -0.5, -0.5);
    cv::filter2D(displayedImage, displayedImage, -1, kernel);
    cv::imshow("Displayed Image", displayedImage);
}

void applyHorizontalEdgeDetect(int, void*) {
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        -1.0, 0.0, 1.0,
        -2.0, 0.0, 2.0,
        -1.0, 0.0, 1.0);
    cv::filter2D(displayedImage, displayedImage, -1, kernel);
    cv::imshow("Displayed Image", displayedImage);
}

void applyVerticalEdgeDetect(int, void*) {
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        -1.0, -2.0, -1.0,
        0.0, 0.0, 0.0,
        1.0, 2.0, 1.0);
    cv::filter2D(displayedImage, displayedImage, -1, kernel);
    cv::imshow("Displayed Image", displayedImage);
}

void resizeImageToFitScreen(const cv::Mat& inputImage, cv::Mat& outputImage, int screenWidth, int screenHeight) {
    double scaleWidth = static_cast<double>(screenWidth) / inputImage.cols;
    double scaleHeight = static_cast<double>(screenHeight) / inputImage.rows;
    double scale = std::min(scaleWidth, scaleHeight);
    cv::resize(inputImage, outputImage, cv::Size(), scale, scale);
}

int main() {
    originalImage = cv::imread("VibrantColor_Sunset.webp");
    if (originalImage.empty()) {
        std::cerr << "Error: Unable to load image. Please check the file path!" << std::endl;
        return -1;
    }

    int screenWidth = 1250;
    int screenHeight = 1000;
    resizeImageToFitScreen(originalImage, resizedImage, screenWidth, screenHeight);
    displayedImage = resizedImage.clone();

    cv::namedWindow("Displayed Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Displayed Image", displayedImage);

    cv::createButton("RESET", applyReset, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Grayscale", applyGrayscaleFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Box Blur", applyBoxBlurFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Gaussian Blur", applyGaussianBlurFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("DOG Filter", applyDifferenceOfGaussiansFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Enhanced DOG Filter", applyEnhancedDifferenceOfGaussiansFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Sharpening", applySharpeningFilter, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Horizontal Edge", applyHorizontalEdgeDetect, nullptr, cv::QT_PUSH_BUTTON, false);
    cv::createButton("Vertical Edge", applyVerticalEdgeDetect, nullptr, cv::QT_PUSH_BUTTON, false);

    cv::waitKey(0);
    return 0;
}
