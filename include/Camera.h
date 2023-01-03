#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


enum Movement_Direction
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 2.5f;
const GLfloat SENSITIVITY = 0.1f;
const GLfloat ZOOM = 45.0f;

/*!
    @brief Класс камеры

    Абстрактный класс камеры, который обрабатывает входные данные и вычисляет соответствующие углы Эйлера, векторы и матрицы для использования в OpenGL
*/
class Camera
{
public:
    // атрибуты камеры
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // углы Эйлера
    GLfloat Yaw;
    GLfloat Pitch;
    // параметры камеры
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    /*!
    * конструктор с параметрами векторов
    */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /*!
    * конструктор с параметрами скалярных значений
    */
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /*!
    * возвращает матрицу вида, рассчитанную с использованием углов Эйлера и матрицы LookAt
    */
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /*!
    * обрабатывает ввод, полученный от любой системы ввода, подобной клавиатуре. Принимает входной параметр в виде определяемого камерой перечисления (чтобы абстрагировать его от оконных систем)
    */
    void ProcessKeyboard(Movement_Direction direction, GLfloat deltaTime)
    {
        GLfloat velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position = Position + Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    /*!
    * обрабатывает ввод, полученный от системы ввода с помощью мыши. Ожидает значение смещения как в направлении x, так и в направлении y.
    */
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // убедитесь, что при превышении высоты тона экран не переворачивается
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // обновляет передний, правый и верхний векторы, используя обновленные углы Эйлера
        updateCameraVectors();
    }

    /*!
    * обрабатывает входные данные, полученные от события колеса прокрутки мыши. Требуется ввод только по вертикальной оси колеса]
    */
    void ProcessMouseScroll(GLfloat yoffset)
    {
        Zoom -= (GLfloat)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    /*!
    * вычисляет вектор фронта по (обновленным) углам Эйлера камеры
    */
    void updateCameraVectors()
    {
        // вычислите новый вектор фронта
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // также пересчитайте вектор вправо и вверх
        Right = glm::normalize(glm::cross(Front, WorldUp));  // нормализуйте векторы, потому что их длина становится ближе к 0, чем больше вы смотрите вверх или вниз, что приводит к замедлению движения.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif