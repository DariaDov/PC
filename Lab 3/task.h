#pragma once

struct Task{
    int id;
    int duration;
    std::function<void(Task)> callback;
};