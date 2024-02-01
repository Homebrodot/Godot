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

RID texture_create_shared(const TextureView &p_view, RID p_with_texture){

}

RID texture_create_from_extension(TextureType p_type, DataFormat p_format, TextureSamples p_samples, BitField<RenderingDevice::TextureUsageBits> p_flags, uint64_t p_image, uint64_t p_width, uint64_t p_height, uint64_t p_depth, uint64_t p_layers){

}

RID texture_create_shared_from_slice(const TextureView &p_view, RID p_with_texture, uint32_t p_layer, uint32_t p_mipmap, uint32_t p_mipmaps = 1, TextureSliceType p_slice_type = TEXTURE_SLICE_2D, uint32_t p_layers = 0){

}

Error RenderingDevice_GX2::texture_update(RID p_texture, uint32_t p_layer, const Vector<uint8_t> &p_data, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS){

}

Vector<uint8_t> RenderingDevice_GX2::texture_get_data(RID p_texture, uint32_t p_layer){

} // CPU textures will return immediately, while GPU textures will most likely force a flush

bool RenderingDevice_GX2::texture_is_format_supported_for_usage(DataFormat p_format, BitField<RenderingDevice::TextureUsageBits> p_usage){
    
}

bool RenderingDevice_GX2::texture_is_shared(RID p_texture){

}

bool RenderingDevice_GX2::texture_is_valid(RID p_texture){

}

TextureFormat RenderingDevice_GX2::texture_get_format(RID p_texture){

}

Size2i RenderingDevice_GX2::texture_size(RID p_texture){

}

uint64_t RenderingDevice_GX2::texture_get_native_handle(RID p_texture){

}

Error RenderingDevice_GX2::texture_copy(RID p_from_texture, RID p_to_texture, const Vector3 &p_from, const Vector3 &p_to, const Vector3 &p_size, uint32_t p_src_mipmap, uint32_t p_dst_mipmap, uint32_t p_src_layer, uint32_t p_dst_layer, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS){

}

Error RenderingDevice_GX2::texture_clear(RID p_texture, const Color &p_color, uint32_t p_base_mipmap, uint32_t p_mipmaps, uint32_t p_base_layer, uint32_t p_layers, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS){

}

Error RenderingDevice_GX2::texture_resolve_multisample(RID p_from_texture, RID p_to_texture, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS){

}
