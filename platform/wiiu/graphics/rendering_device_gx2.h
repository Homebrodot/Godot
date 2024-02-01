#include "servers/rendering/rendering_device.h"

class RenderingDevice_GX2: public RenderingDevice {
public:

	virtual RID texture_create(const TextureFormat &p_format, const TextureView &p_view, const Vector<Vector<uint8_t>> &p_data = Vector<Vector<uint8_t>>()) = 0;
	virtual RID texture_create_shared(const TextureView &p_view, RID p_with_texture) = 0;
	virtual RID texture_create_from_extension(TextureType p_type, DataFormat p_format, TextureSamples p_samples, BitField<RenderingDevice::TextureUsageBits> p_flags, uint64_t p_image, uint64_t p_width, uint64_t p_height, uint64_t p_depth, uint64_t p_layers) = 0;

	virtual RID texture_create_shared_from_slice(const TextureView &p_view, RID p_with_texture, uint32_t p_layer, uint32_t p_mipmap, uint32_t p_mipmaps = 1, TextureSliceType p_slice_type = TEXTURE_SLICE_2D, uint32_t p_layers = 0) = 0;

	virtual Error texture_update(RID p_texture, uint32_t p_layer, const Vector<uint8_t> &p_data, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Vector<uint8_t> texture_get_data(RID p_texture, uint32_t p_layer) = 0; // CPU textures will return immediately, while GPU textures will most likely force a flush

	virtual bool texture_is_format_supported_for_usage(DataFormat p_format, BitField<RenderingDevice::TextureUsageBits> p_usage) const = 0;
	virtual bool texture_is_shared(RID p_texture) = 0;
	virtual bool texture_is_valid(RID p_texture) = 0;
	virtual TextureFormat texture_get_format(RID p_texture) = 0;
	virtual Size2i texture_size(RID p_texture) = 0;
	virtual uint64_t texture_get_native_handle(RID p_texture) = 0;

	virtual Error texture_copy(RID p_from_texture, RID p_to_texture, const Vector3 &p_from, const Vector3 &p_to, const Vector3 &p_size, uint32_t p_src_mipmap, uint32_t p_dst_mipmap, uint32_t p_src_layer, uint32_t p_dst_layer, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Error texture_clear(RID p_texture, const Color &p_color, uint32_t p_base_mipmap, uint32_t p_mipmaps, uint32_t p_base_layer, uint32_t p_layers, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Error texture_resolve_multisample(RID p_from_texture, RID p_to_texture, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;

	virtual FramebufferFormatID framebuffer_format_create_multipass(const Vector<AttachmentFormat> &p_attachments, const Vector<FramebufferPass> &p_passes, uint32_t p_view_count = 1) = 0;
	virtual FramebufferFormatID framebuffer_format_create_empty(TextureSamples p_samples = TEXTURE_SAMPLES_1) = 0;
	virtual TextureSamples framebuffer_format_get_texture_samples(FramebufferFormatID p_format, uint32_t p_pass = 0) = 0;

	virtual RID framebuffer_create(const Vector<RID> &p_texture_attachments, FramebufferFormatID p_format_check = INVALID_ID, uint32_t p_view_count = 1) = 0;
	virtual RID framebuffer_create_multipass(const Vector<RID> &p_texture_attachments, const Vector<FramebufferPass> &p_passes, FramebufferFormatID p_format_check = INVALID_ID, uint32_t p_view_count = 1) = 0;
	virtual RID framebuffer_create_empty(const Size2i &p_size, TextureSamples p_samples = TEXTURE_SAMPLES_1, FramebufferFormatID p_format_check = INVALID_ID) = 0;
	virtual bool framebuffer_is_valid(RID p_framebuffer) const = 0;
	virtual void framebuffer_set_invalidation_callback(RID p_framebuffer, InvalidationCallback p_callback, void *p_userdata) = 0;

	virtual FramebufferFormatID framebuffer_get_format(RID p_framebuffer) = 0;


	virtual RID sampler_create(const SamplerState &p_state) = 0;
	virtual bool sampler_is_format_supported_for_filter(DataFormat p_format, SamplerFilter p_sampler_filter) const = 0;

	virtual RID vertex_buffer_create(uint32_t p_size_bytes, const Vector<uint8_t> &p_data = Vector<uint8_t>(), bool p_use_as_storage = false) = 0;

	typedef int64_t VertexFormatID;

	// This ID is warranted to be unique for the same formats, does not need to be freed
	virtual VertexFormatID vertex_format_create(const Vector<VertexAttribute> &p_vertex_formats) = 0;
	virtual RID vertex_array_create(uint32_t p_vertex_count, VertexFormatID p_vertex_format, const Vector<RID> &p_src_buffers, const Vector<uint64_t> &p_offsets = Vector<uint64_t>()) = 0;

	virtual RID index_buffer_create(uint32_t p_size_indices, IndexBufferFormat p_format, const Vector<uint8_t> &p_data = Vector<uint8_t>(), bool p_use_restart_indices = false) = 0;
	virtual RID index_array_create(RID p_index_buffer, uint32_t p_index_offset, uint32_t p_index_count) = 0;

	virtual bool has_feature(const Features p_feature) const = 0;

	virtual Vector<uint8_t> shader_compile_spirv_from_source(ShaderStage p_stage, const String &p_source_code, ShaderLanguage p_language = SHADER_LANGUAGE_GLSL, String *r_error = nullptr, bool p_allow_cache = true);
	virtual String shader_get_spirv_cache_key() const;

	static void shader_set_compile_to_spirv_function(ShaderCompileToSPIRVFunction p_function);
	static void shader_set_spirv_cache_function(ShaderCacheFunction p_function);
	static void shader_set_get_cache_key_function(ShaderSPIRVGetCacheKeyFunction p_function);

	virtual String shader_get_binary_cache_key() const = 0;
	virtual Vector<uint8_t> shader_compile_binary_from_spirv(const Vector<ShaderStageSPIRVData> &p_spirv, const String &p_shader_name = "") = 0;

	virtual RID shader_create_from_spirv(const Vector<ShaderStageSPIRVData> &p_spirv, const String &p_shader_name = "");
	virtual RID shader_create_from_bytecode(const Vector<uint8_t> &p_shader_binary, RID p_placeholder = RID()) = 0;
	virtual RID shader_create_placeholder() = 0;

	virtual uint64_t shader_get_vertex_input_attribute_mask(RID p_shader) = 0;

	virtual RID uniform_buffer_create(uint32_t p_size_bytes, const Vector<uint8_t> &p_data = Vector<uint8_t>()) = 0;
	virtual RID storage_buffer_create(uint32_t p_size, const Vector<uint8_t> &p_data = Vector<uint8_t>(), BitField<StorageBufferUsage> p_usage = 0) = 0;
	virtual RID texture_buffer_create(uint32_t p_size_elements, DataFormat p_format, const Vector<uint8_t> &p_data = Vector<uint8_t>()) = 0;

	virtual RID uniform_set_create(const Vector<Uniform> &p_uniforms, RID p_shader, uint32_t p_shader_set) = 0;
	virtual bool uniform_set_is_valid(RID p_uniform_set) = 0;
	virtual void uniform_set_set_invalidation_callback(RID p_uniform_set, InvalidationCallback p_callback, void *p_userdata) = 0;

	virtual Error buffer_copy(RID p_src_buffer, RID p_dst_buffer, uint32_t p_src_offset, uint32_t p_dst_offset, uint32_t p_size, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Error buffer_update(RID p_buffer, uint32_t p_offset, uint32_t p_size, const void *p_data, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Error buffer_clear(RID p_buffer, uint32_t p_offset, uint32_t p_size, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual Vector<uint8_t> buffer_get_data(RID p_buffer, uint32_t p_offset = 0, uint32_t p_size = 0) = 0; // This causes stall, only use to retrieve large buffers for saving.

	virtual bool render_pipeline_is_valid(RID p_pipeline) = 0;
	virtual RID render_pipeline_create(RID p_shader, FramebufferFormatID p_framebuffer_format, VertexFormatID p_vertex_format, RenderPrimitive p_render_primitive, const PipelineRasterizationState &p_rasterization_state, const PipelineMultisampleState &p_multisample_state, const PipelineDepthStencilState &p_depth_stencil_state, const PipelineColorBlendState &p_blend_state, BitField<PipelineDynamicStateFlags> p_dynamic_state_flags = 0, uint32_t p_for_render_pass = 0, const Vector<PipelineSpecializationConstant> &p_specialization_constants = Vector<PipelineSpecializationConstant>()) = 0;

	virtual RID compute_pipeline_create(RID p_shader, const Vector<PipelineSpecializationConstant> &p_specialization_constants = Vector<PipelineSpecializationConstant>()) = 0;
	virtual bool compute_pipeline_is_valid(RID p_pipeline) = 0;

	virtual int screen_get_width(DisplayServer::WindowID p_screen = 0) const = 0;
	virtual int screen_get_height(DisplayServer::WindowID p_screen = 0) const = 0;
	virtual FramebufferFormatID screen_get_framebuffer_format() const = 0;

	virtual DrawListID draw_list_begin_for_screen(DisplayServer::WindowID p_screen = 0, const Color &p_clear_color = Color()) = 0;
	virtual DrawListID draw_list_begin(RID p_framebuffer, InitialAction p_initial_color_action, FinalAction p_final_color_action, InitialAction p_initial_depth_action, FinalAction p_final_depth_action, const Vector<Color> &p_clear_color_values = Vector<Color>(), float p_clear_depth = 1.0, uint32_t p_clear_stencil = 0, const Rect2 &p_region = Rect2(), const Vector<RID> &p_storage_textures = Vector<RID>()) = 0;
	virtual Error draw_list_begin_split(RID p_framebuffer, uint32_t p_splits, DrawListID *r_split_ids, InitialAction p_initial_color_action, FinalAction p_final_color_action, InitialAction p_initial_depth_action, FinalAction p_final_depth_action, const Vector<Color> &p_clear_color_values = Vector<Color>(), float p_clear_depth = 1.0, uint32_t p_clear_stencil = 0, const Rect2 &p_region = Rect2(), const Vector<RID> &p_storage_textures = Vector<RID>()) = 0;

	virtual void draw_list_set_blend_constants(DrawListID p_list, const Color &p_color) = 0;
	virtual void draw_list_bind_render_pipeline(DrawListID p_list, RID p_render_pipeline) = 0;
	virtual void draw_list_bind_uniform_set(DrawListID p_list, RID p_uniform_set, uint32_t p_index) = 0;
	virtual void draw_list_bind_vertex_array(DrawListID p_list, RID p_vertex_array) = 0;
	virtual void draw_list_bind_index_array(DrawListID p_list, RID p_index_array) = 0;
	virtual void draw_list_set_line_width(DrawListID p_list, float p_width) = 0;
	virtual void draw_list_set_push_constant(DrawListID p_list, const void *p_data, uint32_t p_data_size) = 0;

	virtual void draw_list_draw(DrawListID p_list, bool p_use_indices, uint32_t p_instances = 1, uint32_t p_procedural_vertices = 0) = 0;

	virtual void draw_list_enable_scissor(DrawListID p_list, const Rect2 &p_rect) = 0;
	virtual void draw_list_disable_scissor(DrawListID p_list) = 0;

	virtual uint32_t draw_list_get_current_pass() = 0;
	virtual DrawListID draw_list_switch_to_next_pass() = 0;
	virtual Error draw_list_switch_to_next_pass_split(uint32_t p_splits, DrawListID *r_split_ids) = 0;

	virtual void draw_list_end(BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;

	virtual ComputeListID compute_list_begin(bool p_allow_draw_overlap = false) = 0;
	virtual void compute_list_bind_compute_pipeline(ComputeListID p_list, RID p_compute_pipeline) = 0;
	virtual void compute_list_bind_uniform_set(ComputeListID p_list, RID p_uniform_set, uint32_t p_index) = 0;
	virtual void compute_list_set_push_constant(ComputeListID p_list, const void *p_data, uint32_t p_data_size) = 0;
	virtual void compute_list_dispatch(ComputeListID p_list, uint32_t p_x_groups, uint32_t p_y_groups, uint32_t p_z_groups) = 0;
	virtual void compute_list_dispatch_threads(ComputeListID p_list, uint32_t p_x_threads, uint32_t p_y_threads, uint32_t p_z_threads) = 0;
	virtual void compute_list_dispatch_indirect(ComputeListID p_list, RID p_buffer, uint32_t p_offset) = 0;
	virtual void compute_list_add_barrier(ComputeListID p_list) = 0;

	virtual void compute_list_end(BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS) = 0;

	virtual void barrier(BitField<BarrierMask> p_from = BARRIER_MASK_ALL_BARRIERS, BitField<BarrierMask> p_to = BARRIER_MASK_ALL_BARRIERS) = 0;
	virtual void full_barrier() = 0;

	virtual void free(RID p_id) = 0;

	virtual void capture_timestamp(const String &p_name) = 0;
	virtual uint32_t get_captured_timestamps_count() const = 0;
	virtual uint64_t get_captured_timestamps_frame() const = 0;
	virtual uint64_t get_captured_timestamp_gpu_time(uint32_t p_index) const = 0;
	virtual uint64_t get_captured_timestamp_cpu_time(uint32_t p_index) const = 0;
	virtual String get_captured_timestamp_name(uint32_t p_index) const = 0;


	virtual uint64_t get_memory_usage(MemoryType p_type) const = 0;

	virtual RenderingDevice *create_local_device() = 0;

	virtual void set_resource_name(RID p_id, const String p_name) = 0;

	virtual void draw_command_begin_label(String p_label_name, const Color p_color = Color(1, 1, 1, 1)) = 0;
	virtual void draw_command_insert_label(String p_label_name, const Color p_color = Color(1, 1, 1, 1)) = 0;
	virtual void draw_command_end_label() = 0;

	virtual String get_device_vendor_name() const = 0;
	virtual String get_device_name() const = 0;
	virtual RenderingDevice::DeviceType get_device_type() const = 0;
	virtual String get_device_api_version() const = 0;
	virtual String get_device_pipeline_cache_uuid() const = 0;

	virtual uint64_t get_driver_resource(DriverResource p_resource, RID p_rid = RID(), uint64_t p_index = 0) = 0;

	static RenderingDevice *get_singleton();
	RenderingDevice();

protected:
	static const char *shader_stage_names[RenderingDevice::SHADER_STAGE_MAX];

	static const uint32_t MAX_UNIFORM_SETS = 16;

	//binders to script API
	RID _texture_create(const Ref<RDTextureFormat> &p_format, const Ref<RDTextureView> &p_view, const TypedArray<PackedByteArray> &p_data = Array());
	RID _texture_create_shared(const Ref<RDTextureView> &p_view, RID p_with_texture);
	RID _texture_create_shared_from_slice(const Ref<RDTextureView> &p_view, RID p_with_texture, uint32_t p_layer, uint32_t p_mipmap, uint32_t p_mipmaps = 1, TextureSliceType p_slice_type = TEXTURE_SLICE_2D);
	Ref<RDTextureFormat> _texture_get_format(RID p_rd_texture);

	FramebufferFormatID _framebuffer_format_create(const TypedArray<RDAttachmentFormat> &p_attachments, uint32_t p_view_count);
	FramebufferFormatID _framebuffer_format_create_multipass(const TypedArray<RDAttachmentFormat> &p_attachments, const TypedArray<RDFramebufferPass> &p_passes, uint32_t p_view_count);
	RID _framebuffer_create(const TypedArray<RID> &p_textures, FramebufferFormatID p_format_check = INVALID_ID, uint32_t p_view_count = 1);
	RID _framebuffer_create_multipass(const TypedArray<RID> &p_textures, const TypedArray<RDFramebufferPass> &p_passes, FramebufferFormatID p_format_check = INVALID_ID, uint32_t p_view_count = 1);
	RID _sampler_create(const Ref<RDSamplerState> &p_state);
	VertexFormatID _vertex_format_create(const TypedArray<RDVertexAttribute> &p_vertex_formats);
	RID _vertex_array_create(uint32_t p_vertex_count, VertexFormatID p_vertex_format, const TypedArray<RID> &p_src_buffers, const Vector<int64_t> &p_offsets = Vector<int64_t>());

	Ref<RDShaderSPIRV> _shader_compile_spirv_from_source(const Ref<RDShaderSource> &p_source, bool p_allow_cache = true);
	Vector<uint8_t> _shader_compile_binary_from_spirv(const Ref<RDShaderSPIRV> &p_bytecode, const String &p_shader_name = "");
	RID _shader_create_from_spirv(const Ref<RDShaderSPIRV> &p_spirv, const String &p_shader_name = "");

	RID _uniform_set_create(const TypedArray<RDUniform> &p_uniforms, RID p_shader, uint32_t p_shader_set);

	Error _buffer_update(RID p_buffer, uint32_t p_offset, uint32_t p_size, const Vector<uint8_t> &p_data, BitField<BarrierMask> p_post_barrier = BARRIER_MASK_ALL_BARRIERS);

	RID _render_pipeline_create(RID p_shader, FramebufferFormatID p_framebuffer_format, VertexFormatID p_vertex_format, RenderPrimitive p_render_primitive, const Ref<RDPipelineRasterizationState> &p_rasterization_state, const Ref<RDPipelineMultisampleState> &p_multisample_state, const Ref<RDPipelineDepthStencilState> &p_depth_stencil_state, const Ref<RDPipelineColorBlendState> &p_blend_state, BitField<PipelineDynamicStateFlags> p_dynamic_state_flags, uint32_t p_for_render_pass, const TypedArray<RDPipelineSpecializationConstant> &p_specialization_constants);
	RID _compute_pipeline_create(RID p_shader, const TypedArray<RDPipelineSpecializationConstant> &p_specialization_constants);

	DrawListID _draw_list_begin(RID p_framebuffer, InitialAction p_initial_color_action, FinalAction p_final_color_action, InitialAction p_initial_depth_action, FinalAction p_final_depth_action, const Vector<Color> &p_clear_color_values = Vector<Color>(), float p_clear_depth = 1.0, uint32_t p_clear_stencil = 0, const Rect2 &p_region = Rect2(), const TypedArray<RID> &p_storage_textures = TypedArray<RID>());
	Vector<int64_t> _draw_list_begin_split(RID p_framebuffer, uint32_t p_splits, InitialAction p_initial_color_action, FinalAction p_final_color_action, InitialAction p_initial_depth_action, FinalAction p_final_depth_action, const Vector<Color> &p_clear_color_values = Vector<Color>(), float p_clear_depth = 1.0, uint32_t p_clear_stencil = 0, const Rect2 &p_region = Rect2(), const TypedArray<RID> &p_storage_textures = TypedArray<RID>());
	void _draw_list_set_push_constant(DrawListID p_list, const Vector<uint8_t> &p_data, uint32_t p_data_size);
	void _compute_list_set_push_constant(ComputeListID p_list, const Vector<uint8_t> &p_data, uint32_t p_data_size);
	Vector<int64_t> _draw_list_switch_to_next_pass_split(uint32_t p_splits);

	Error _reflect_spirv(const Vector<ShaderStageSPIRVData> &p_spirv, SpirvReflectionData &r_reflection_data);

}