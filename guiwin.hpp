#pragma once

#include "imgui/imgui.h"
#include <stdio.h>

#include <alliedcam.h>

#include "imagetexture.hpp"

class ImageDisplay
{
public:
    bool show;

    ImageDisplay()
    {
        show = false;
    }

    void display(Image *img)
    {
        ImGui::Begin("Image Display", &show);
        ImGui::Text("Image Display");
        GLuint texture = 0;
        uint32_t width = 0, height = 0;
        if (show)
        {
            img->get_texture(texture, width, height);
        }
        ImGui::Text("Texture: %d | %u x %u", texture, width, height);
        if (show)
        {
            ImGui::Image((void *)(intptr_t)texture, ImVec2(width, height));
        }
        ImGui::End();
    }
};