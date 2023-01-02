#include "gui.hpp"
#include "common_defines.hpp"
// #include <windows.h>
GUI *GUI::instance;

void GUI::render(argb_texture back_buffer, assets &asset_cache)
{
    GUI_Element *elem = nullptr;
    Linked_List_Iterator<GUI_Element> *iter = this->list_builder->build();
    while (elem = iter->next())
    {
        switch (elem->tag)
        {
        case Element_Tag::BUTTON:
            {
            GUI_Button *btn = (GUI_Button *)elem;
            const char *text = btn->activated ? "Inactive":"Active";
            asset_cache.font_1.DEBUGrender_bitmap_text(&back_buffer, (char *)text, elem->position.x, elem->position.y);
            }
            break;
        case Element_Tag::TEXT:
            asset_cache.font_1.DEBUGrender_bitmap_text(&back_buffer, ((GUI_Text *)elem)->text, elem->position.x, elem->position.y);
            break;
        default:
            throw new Not_Implemented_Exception();
        }
    }
    delete iter;
}