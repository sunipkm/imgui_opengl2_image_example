#pragma once
#include <iostream>
#include "imgui/imgui.h"
#include <stdio.h>

#include <alliedcam.h>

#include "imagetexture.hpp"

class ImageDisplay
{
private:
    ImVec2 render_size(uint32_t swid, uint32_t shgt)
    {
        ImVec2 avail_size = ImGui::GetContentRegionAvail();
        float wid = avail_size[0]; // get window width
        float hgt = avail_size[1]; // get window height
        ImVec2 out = ImVec2(
            wid,
            round((float)shgt / (float)swid * wid) // calculate height
        );
        if (out[1] > hgt)
        {
            out[1] = hgt;
            out[0] = round((float)swid / (float)shgt * hgt); // calculate width
        }
        return out;
    }

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
        ImGui::Text("Texture: %d | %u x %u | %d", texture, width, height, (int)ImGui::GetWindowWidth());
        if (show)
        {
            ImGui::Image((void *)(intptr_t)texture, render_size(width, height));
        }
        ImGui::End();
    }
};