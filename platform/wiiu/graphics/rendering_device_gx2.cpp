#include "rendering_device_gx2.h"

//"I paid for the whole GX2, I'm gonna use the whole GX2."
#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/displaylist.h>
#include <gx2/draw.h>
#include <gx2/enum.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/sampler.h>
#include <gx2/shaders.h>
#include <gx2/state.h>
#include <gx2/state.h>
#include <gx2/surface.h>
#include <gx2/swap.h>
#include <gx2/temp.h>
#include <gx2/tessellation.h>
#include <gx2/texture.h>
#include <gx2/utils.h>

RID RenderingDevice_GX2::texture_create(const TextureFormat &p_format, const TextureView &p_view, const Vector<Vector<uint8_t>> &p_data){
    GX2Surface newsurface;
    newsurface.width = p_format.width;
    newsurface.height = p_format.height;
    newsurface.depth = p_format.depth;
    newsurface.mipmaps = p_format.mipmaps;
    

}