#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <windows.h>
#include <Xinput.h>

using namespace std;
using namespace cv;

void sendKey(WORD key, bool press) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = key;
    input.ki.dwFlags = press ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void controlDirection(double angle, int& lastDirectionKey) {
    int newDirectionKey = 0;

    if (angle < -30) {
        newDirectionKey = VK_LEFT;
    }
    else if (angle > 30) {
        newDirectionKey = VK_RIGHT;
    }

    if (newDirectionKey != lastDirectionKey) {
        if (lastDirectionKey != 0) {
            sendKey(lastDirectionKey, false);
            cout << "Liberando tecla de direco." << endl;
        }
        if (newDirectionKey != 0) {
            cout << "Pressionando tecla de direcao" << endl;
            sendKey(newDirectionKey, true);
        }
        lastDirectionKey = newDirectionKey;
    }
}

void controlBrakeNitro(const Point& middle_point, int height, int& lastBrakeKey, int& lastNitroKey, int width) {
    bool isInBrakeZone = (middle_point.y > height / 3 && middle_point.y < 2 * height / 3);
    bool isInNitroZone = (middle_point.y > 2 * height / 3) && (middle_point.x < width / 3 || middle_point.x > 2 * width / 3);

    if (isInBrakeZone) {
        if (lastBrakeKey != VK_DOWN) {
            cout << "Pressionando seta para baixo." << endl;
            sendKey(VK_DOWN, true);
            lastBrakeKey = VK_DOWN;
        }

        if (lastNitroKey != 0) {
            sendKey(VK_SPACE, false);
            cout << "Liberando nitro." << endl;
            lastNitroKey = 0;
        }
    }
    else if (isInNitroZone) {
        if (lastNitroKey != VK_SPACE) {
            cout << "Pressionando espaço." << endl;
            sendKey(VK_SPACE, true);
            lastNitroKey = VK_SPACE;
        }
    }
    else {

        if (lastBrakeKey != 0) {
            sendKey(VK_DOWN, false);
            cout << "Liberando seta para baixo." << endl;
            lastBrakeKey = 0;
        }
        if (lastNitroKey != 0) {
            sendKey(VK_SPACE, false);
            cout << "Liberando espaço." << endl;
            lastNitroKey = 0;
        }
    }
}


class Menu {
public:
    void displayMenu() {
        cout << "\n--- Menu ---\n";
        cout << "1. Iniciar Volante Virtual\n";
        cout << "2. Sair\n";
        cout << "Escolha uma opcao: ";
    }

    void menuOption(bool& running) {
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            startSteering();
            break;
        case 2:
            cout << "Encerrando..." << endl;
            running = false;
            break;
        default:
            cout << "Opcao invalida! Tente novamente." << endl;
            break;
        }
    }

private:
    void startSteering() {
        VideoCapture cap(0);

        if (!cap.isOpened()) {
            cerr << "Erro ao abrir a câmera!" << endl;
            return;
        }

        Mat frame, hsv, mask_yellow, mask_magenta;
        Scalar lower_yellow(20, 100, 100);
        Scalar upper_yellow(30, 255, 255);
        Scalar lower_magenta(140, 120, 120);
        Scalar upper_magenta(190, 255, 255);

        int lastDirectionKey = 0;
        int lastBrakeKey = 0;
        int lastNitroKey = 0;
        bool detected = false;

        while (true) {
            cap >> frame;
            if (frame.empty()) {
                cerr << "Frame vazio!" << endl;
                break;
            }

            flip(frame, frame, 1);
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            inRange(hsv, lower_yellow, upper_yellow, mask_yellow);
            inRange(hsv, lower_magenta, upper_magenta, mask_magenta);

            Moments m_yellow = moments(mask_yellow, true);
            Moments m_magenta = moments(mask_magenta, true);

            Point center_yellow(m_yellow.m10 / (m_yellow.m00 + 1e-5), m_yellow.m01 / (m_yellow.m00 + 1e-5));
            Point center_magenta(m_magenta.m10 / (m_magenta.m00 + 1e-5), m_magenta.m01 / (m_magenta.m00 + 1e-5));

            detected = false;
            if (m_yellow.m00 > 0) {
                circle(frame, center_yellow, 5, Scalar(0, 255, 255), -1);
                detected = true;
            }
            if (m_magenta.m00 > 0) {
                circle(frame, center_magenta, 5, Scalar(255, 0, 255), -1);
                detected = true;
            }

            if (m_yellow.m00 > 0 && m_magenta.m00 > 0) {
                Point middle_point((center_yellow.x + center_magenta.x) / 2, (center_yellow.y + center_magenta.y) / 2);

                line(frame, center_yellow, center_magenta, Scalar(0, 0, 0), 2);
                circle(frame, middle_point, 10, Scalar(0, 255, 0), -1);

                double angle = atan2(center_magenta.y - center_yellow.y, center_magenta.x - center_yellow.x) * 180.0 / CV_PI;
                cout << "Ângulo: " << angle << endl;
                controlDirection(angle, lastDirectionKey);

                controlBrakeNitro(middle_point, frame.rows, lastBrakeKey, lastNitroKey, frame.cols);
            }

            int height = frame.rows;
            int width = frame.cols;
            line(frame, Point(0, height / 3), Point(width, height / 3), Scalar(0, 0, 0), 2);
            line(frame, Point(0, 2 * height / 3), Point(width, 2 * height / 3), Scalar(0, 0, 0), 2);
            putText(frame, "DRIFT", Point(width / 2 - 30, height / 2 + 10), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

            line(frame, Point(width / 3, 2 * height / 3), Point(width / 3, height), Scalar(0, 0, 0), 2);
            line(frame, Point(2 * width / 3, 2 * height / 3), Point(2 * width / 3, height), Scalar(0, 0, 0), 2);
            putText(frame, "NOS", Point(width / 6 - 15, 5 * height / 6), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
            putText(frame, "NOS", Point(3 * width / 4 - 15, 5 * height / 6), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

            putText(frame, "Pressione ESC para sair", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
            imshow("Volante Virtual", frame);

            if (waitKey(30) == 27) {
                break;
            }
        }
    }
};

int main() {
    Menu menu;
    bool running = true;

    while (running) {
        menu.displayMenu();
        menu.menuOption(running);
    }

    return 0;
}
