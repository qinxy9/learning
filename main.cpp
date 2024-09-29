#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip> // 用于设置输出格式

// 重力加速度
const float gravity = 0.5f;
// 向上速度
const float jumpSpeed = -6.0f; // 改为6
// 时间步长
const int timeStep = 20;

// 墙的参数
const int wallWidth = 20;   // 墙的宽度
const int gapWidth = 120;    // 墙缺口的宽度（调整为90 + 30 = 120像素）
const float wallSpacing = 300.0f; // 墙之间的距离（原来的1.5倍）
const int wallSpeed = 2;     // 墙的移动速度

struct Wall {
    int x;    // 墙的X位置
    int gapY; // 墙缺口的Y位置
};

int main() {
    // 窗口尺寸
    const int windowWidth = 1200;
    const int windowHeight = 800;

    // 地图矩形尺寸
    const int mapWidth = 1000;
    const int mapHeight = 600;

    // 小球的尺寸
    const int ballDiameter = 30;

    // 初始位置和速度
    float ballY;
    float ballSpeedY = 0.0f;  // 小球垂直速度
    bool isMoving = false;     // 小球是否开始移动
    bool gameOver = false;     // 游戏是否结束

    // 创建一个黑色背景图像
    cv::Mat window(windowHeight, windowWidth, CV_8UC3, cv::Scalar(0, 0, 0));

    // 计算地图矩形的左上角坐标（居中）
    int mapX = (windowWidth - mapWidth) / 2;
    int mapY = (windowHeight - mapHeight) / 2;

    // 小球初始位置（靠近左侧1/4处并向下偏移60像素）
    ballY = mapY + ballDiameter / 2 + 20 + 60;  // 垂直方向偏移60像素
    int ballX = mapX + mapWidth / 4;            // 靠近左侧1/4处

    // 墙数据结构
    std::vector<Wall> walls;

    // 初始化随机数种子
    std::srand(std::time(nullptr));

    // 定时器设置
    int frameCount = 0;
    const int printInterval = 250; // 每250ms打印一次
    float totalTime = 0.0f; // 总时间

    // 游戏循环
    while (true) {
        // 处理键盘输入
        int key = cv::waitKey(timeStep);
        if (key == 27) { // 按下 ESC 退出
            break;
        } else if (key == '1' && !gameOver) { // 按下数字"1"键
            isMoving = true;  // 按下“1”后开始移动
            ballSpeedY = jumpSpeed;
        }

        if (isMoving && !gameOver) {
            // 更新小球位置和速度
            ballSpeedY += gravity;    // 受重力加速度影响
            ballY += ballSpeedY;      // 更新位置

            // 边界检测，防止小球超出屏幕
            if (ballY + ballDiameter / 2 >= mapY + mapHeight) {
                ballY = mapY + mapHeight - ballDiameter / 2;  // 底部边界
                ballSpeedY = 0;  // 停止
            } else if (ballY - ballDiameter / 2 <= mapY) {
                ballY = mapY + ballDiameter / 2;  // 顶部边界
                ballSpeedY = 0;  // 停止
            }

            // 创建新墙：如果墙的间隔达到条件，则生成一个新墙
            if (walls.empty() || (walls.back().x < mapX + mapWidth - wallSpacing)) {
                Wall newWall;
                newWall.x = mapX + mapWidth;  // 墙从地图右边生成
                newWall.gapY = mapY + (std::rand() % (mapHeight - gapWidth));  // 随机生成缺口位置
                walls.push_back(newWall);
            }

            // 更新墙的位置，并检查是否有墙超出左边界
            for (auto& wall : walls) {
                wall.x -= wallSpeed;  // 墙向左移动
            }
            if (!walls.empty() && walls.front().x + wallWidth < mapX) {
                walls.erase(walls.begin());  // 删除超出左边界的墙
            }

            // 碰撞检测
            for (const auto& wall : walls) {
                if (ballX + ballDiameter / 2 > wall.x && ballX - ballDiameter / 2 < wall.x + wallWidth) {
                    if (ballY - ballDiameter / 2 < wall.gapY || ballY + ballDiameter / 2 > wall.gapY + gapWidth) {
                        gameOver = true;  // 小球撞到墙
                    }
                }
            }

            // 计算最近的墙
            if (!walls.empty()) {
                Wall& nearestWall = walls.front();
                float distance = nearestWall.x - (ballX + ballDiameter / 2); // 小球中心到墙的横向距离
                float gapCenterY = nearestWall.gapY + gapWidth / 2; // 墙缺口的中心高度

                // 每250ms打印信息
                if (frameCount % (printInterval / timeStep) == 0) {
                    std::cout << std::fixed << std::setprecision(2); // 设置输出格式
                    std::cout << "总时间: " << totalTime << " 秒, "; // 输出总时间
                    std::cout << "距离最近的墙: " << distance << " 像素, ";
                    std::cout << "小球高度: " << ballY - (mapY + ballDiameter / 2) << " 像素, ";
                    std::cout << "小球速度: " << ballSpeedY << " 像素/s, ";
                    std::cout << "最近墙缺口中心高度: " << gapCenterY - (mapY + ballDiameter / 2) << " 像素" << std::endl;
                }
            }

            totalTime += timeStep / 1000.0f; // 增加总时间
            frameCount++;
        }

        // 清空窗口
        window.setTo(cv::Scalar(0, 0, 0));

        // 绘制地图矩形
        cv::rectangle(window, cv::Point(mapX, mapY), cv::Point(mapX + mapWidth, mapY + mapHeight), cv::Scalar(255, 255, 255), 2);

        // 绘制小球
        cv::circle(window, cv::Point(ballX, ballY), ballDiameter / 2, cv::Scalar(0, 255, 0), -1);

        // 绘制墙
        for (const auto& wall : walls) {
            // 绘制墙体
            cv::rectangle(window, cv::Point(wall.x, mapY), cv::Point(wall.x + wallWidth, wall.gapY), cv::Scalar(255, 0, 0), -1);
            cv::rectangle(window, cv::Point(wall.x, wall.gapY + gapWidth), cv::Point(wall.x + wallWidth, mapY + mapHeight), cv::Scalar(255, 0, 0), -1);
        }

        // 如果游戏结束，显示 Game Over
        if (gameOver) {
            cv::putText(window, "Game Over", cv::Point(windowWidth / 2 - 100, windowHeight / 2), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
        }

        // 显示窗口
        cv::imshow("Window", window);
    }

    return 0;
}
