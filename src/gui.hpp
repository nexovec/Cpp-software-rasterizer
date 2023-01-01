#pragma once
#include <iostream>

// #include <vector>
#include "super_math.hpp"

class Input{
    public:
    explicit Input(){}
};
class Mouse_Input : Input{
};

class Keyboard_Input : Input{
};

class GUI_Element{
    vec2_f position;
    vec2_f size;
};
class GUI_Button : GUI_Element{
};
class GUI_Checkbox: GUI_Element{
};

template <typename T>
class IReader{
    virtual T& read() = 0;
};

template <typename T>
class IWriter{
    virtual bool write(T& ) = 0;// TODO: test if it runs a copy constructor if you don't use the reference
};

template <typename T>
class Channel : IReader<T>, IWriter<T>{
};

template <typename T>
class IQueue{
    public:
    virtual void enqueue(T& t) = 0;
    virtual T& dequeue() = 0;
};

template <typename T>
class Iterator{
    virtual T* next() = 0;
};
class GUI_Elems_Iterator{
    virtual GUI_Element *next();
};

class GUI_Elements_Queue : IQueue<GUI_Element>{
    void enqueue();
};

class Rendering_Strategy{
    virtual void render_queue(GUI_Elems_Iterator it);
};

class GUI{
    static GUI* instance;
    public:
    GUI(){
        // std::cout << "Creating GUI singleton" << std::endl;
    }
    static const GUI* get(){
        if(!GUI::instance)
            GUI::instance = new GUI();
        return GUI::instance;
    }
    void pass_input(Input input);
    void render(Rendering_Strategy renderer);
};