#ifndef __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP
#define __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP

#include "task_controller.hpp"

#include <array>


template <typename T, std::size_t N>
class ValueBlender
{
public:
    ValueBlender() : _values(), _blends() {
        for(auto &value: this->_values) {
            value = static_cast<T>(0);
        }
        for(auto &blend: this->_blends) {
            blend = 0.0;
        }

        // index=0の値を選択状態にする
        this->selectIndex(0, 1.0);
        this->get_blended();
    };

    void setValues(std::size_t index, T value) {
        this->_values[index] = value;
    };

    void selectIndex(std::size_t index, float blend_speed) {
        this->_current_index = index;
        this->_current_blend_speed = blend_speed;
    };

    T get_blended() {
        if (this->_blends[this->_current_index] >= 1.0) {
            return this->_values[this->_current_index];
        }

        // 選択されたインデックスのブレンド値を上昇させる
        this->_blends[this->_current_index] += this->_current_blend_speed;

        // ブレンド値が1.0を超えた場合は、他のインデックスのブレンド値を0にして、
        // 選択されたインデックスのブレンド値を1.0にし、その値を返す
        if (this->_blends[this->_current_index] >= 1.0) {
            for (auto &blend: this->_blends) blend = 0.0;
            this->_blends[this->_current_index] = 1.0;
            return this->_values[this->_current_index];
        }

        // 現在の割合のまま減少させる
        float next_remain = 1.0 - this->_blends[this->_current_index];
        float current_remain = 0.0;
        for (int i = 0; i < N; i++) {
            if (i == this->_current_index) continue;
            current_remain += this->_blends[i];
        }
        for (auto &blend: this->_blends) {
            blend *= next_remain / current_remain;
        }

        // 現在のブレンド値を返す
        T result = static_cast<T>(0);
        for (int i = 0; i < N; i++) {
            result += static_cast<T>(this->_values[i] * this->_blends[i]);
        }
        return result;
    }

private:
    std::array<T, N> _values;
    std::array<float, N> _blends;
    std::size_t _current_index;
    int _current_blend_speed;
};


class ControlLoopContext : public AbstractRtosTaskContext
{
public:
    ControlLoopContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();

private:
    ValueBlender<int32_t, 5> _blender;
};



#endif // __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP
