#include <string.h>

#include "pco.h"

static void _decode_line(int width, void *bufout, void *bufin)
{
    uint32_t *lineadr_in = (uint32_t *)bufin;
    uint32_t *lineadr_out = (uint32_t *)bufout;
    uint32_t a;

    for (int x = 0; x < (width * 12) / 32; x += 3)
    {
        a = (*lineadr_in & 0x0000FFF0) >> 4;
        a |= (*lineadr_in & 0x0000000F) << 24;
        a |= (*lineadr_in & 0xFF000000) >> 8;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0x00FF0000) >> 12;
        lineadr_in++;
        a |= (*lineadr_in & 0x0000F000) >> 12;
        a |= (*lineadr_in & 0x00000FFF) << 16;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0xFFF00000) >> 20;
        a |= (*lineadr_in & 0x000F0000) << 8;
        lineadr_in++;
        a |= (*lineadr_in & 0x0000FF00) << 8;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0x000000FF) << 4;
        a |= (*lineadr_in & 0xF0000000) >> 28;
        a |= (*lineadr_in & 0x0FFF0000);
        *lineadr_out = a;
        lineadr_out++;
        lineadr_in++;
    }
}

void func_edge_reorder_image_5x12(uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint16_t *line_top = bufout;
    uint16_t *line_bottom = bufout + (height - 1) * width;
    uint16_t *line_in = bufin;
    int off = (width * 12) / 16;

    for (int y = 0; y < height / 2; y++)
    {
        _decode_line(width, line_top, line_in);
        line_in += off;
        _decode_line(width, line_bottom, line_in);
        line_in += off;
        line_top += width;
        line_bottom -= width;
    }
}

void func_edge_reorder_image_5x16(uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint16_t *line_top = bufout;
    uint16_t *line_bottom = bufout + (height - 1) * width;
    uint16_t *line_in = bufin;

    for (int y = 0; y < height / 2; y++)
    {
        memcpy(line_top, line_in, width * sizeof(uint16_t));
        line_in += width;
        memcpy(line_bottom, line_in, width * sizeof(uint16_t));
        line_in += width;
        line_top += width;
        line_bottom -= width;
    }
}