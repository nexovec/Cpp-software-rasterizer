#pragma once
#include <iostream>
#include <cassert>

// #include <vector>
#include "super_math.hpp"
#include "asset_API.hpp"
#include "common_defines.hpp"
#include <windows.h>

class Input
{
public:
    explicit Input() {}
};
class Mouse_Input : Input
{
};

class Keyboard_Input : Input
{
};

enum Element_Tag
{
    BUTTON = 0,
    TEXT
};
class GUI_Element
{
protected:
    GUI_Element(){};

public:
    Element_Tag tag;
    vec2_f position;

protected:
    GUI_Element(Element_Tag tag, vec2_f position) : tag(tag), position(position){};
};
class GUI_Button : public GUI_Element
{
public:
    vec2_f size;
    bool activated;
    GUI_Button(vec2_f position, vec2_f size) : GUI_Element(Element_Tag::BUTTON, position), size(size){};
};
class GUI_Text : public GUI_Element
{
    int size;

public:
    char *text;
    GUI_Text(vec2_f position, int size, char *text) : GUI_Element(Element_Tag::TEXT, position), size(size), text(text){};
};

template <typename T>
class IReader
{
    virtual T &read() = 0;
};

template <typename T>
class IWriter
{
    virtual bool write(T &) = 0; // TODO: test if it runs a copy constructor if you don't use the reference
};

template <typename T>
class Channel : IReader<T>, IWriter<T>
{
};

template <typename T>
class IQueue
{
public:
    virtual void enqueue(T &t) = 0;
    virtual T &dequeue() = 0;
};

template <typename T>
class Iterator
{
    virtual T *next() = 0;
    virtual T *first() = 0;
};

// can be a composite pattern
template <class T>
class List_Node
{
public:
    T *current;
    List_Node<T> *next;
    List_Node<T>(T *item, List_Node<T> *next) : current(item), next(next)
    {
    }
};

template <class T>
class Linked_List_Iterator : public Iterator<T>
{
    List_Node<T> *root;
    List_Node<T> *cursor;

public:
    Linked_List_Iterator(List_Node<T> *root_node) : root(root_node), cursor(root_node)
    {
        assert(root_node != nullptr);
    }
    virtual T *next()
    {
        if (cursor == nullptr)
        {
            return nullptr;
        }
        T *item = cursor->current;
        cursor = cursor->next;
        return item;
    }
    virtual T *first()
    {
        if (root == nullptr)
            return nullptr;
        return root->current;
    }
};

template <class T>
class Linked_List_Builder
{
    List_Node<T> *root;
    List_Node<T> wrap(T *item)
    {
        return List_Node(item, nullptr);
    }

public:
    void push(List_Node<T> *node)
    {
        if (this->root == nullptr)
        {
            this->root = node;
            return;
        }
        // return this->root = node;
        List_Node<T> *cursor = root;
        while (cursor->next != nullptr)
        {
            cursor = cursor->next;
        }
        cursor->next = node;
        // return node;
    }
    void push(T *item)
    {
        auto node = new List_Node<T>(item, nullptr);
        this->push(node);
    }
    void clear()
    {
        // HACK, causes a memory-leak
        root = nullptr;
    }
    // template <class T>
    Linked_List_Iterator<T> *build()
    {
        return new Linked_List_Iterator<T>(this->root);
    }
};

// singleton, facade
class GUI
{
    bool frozen = false;
    Linked_List_Builder<GUI_Element> *list_builder;
    static GUI *instance;
    GUI()
    {
        list_builder = new Linked_List_Builder<GUI_Element>();
    }

public:
    static GUI *get()
    {
        if (!GUI::instance)
            GUI::instance = new GUI();
        return GUI::instance;
    }
    void add_ui(GUI_Element *elem)
    {
        if (frozen)
        {
            delete elem;
            return;
        }
        this->list_builder->push(elem);
    }
    void pass_input(Input input);
    void render(argb_texture back_buffer, assets &asset_cache);
    void freeze()
    {
        frozen = true;
    }
    void cleanup()
    {
        list_builder->clear();
    }
};