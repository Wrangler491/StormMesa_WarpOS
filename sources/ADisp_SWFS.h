#ifndef _ADISP_SWFS_H
#define _ADISP_SWFS_H

BOOL SWFSDriver_init(struct amigamesa_context *c,struct TagItem *tagList);
void ChunkyToPlanar(unsigned char* src,struct BitMap *dest,int width,int height);
void SetBuffer(AmigaMesaContext c);
void SWFSDriver_SwapBuffer_AGA(struct amigamesa_context *c);
void SWFSDriver_SwapBuffer_FS(struct amigamesa_context *c);
void SWFSDriver_SwapBuffer_FS3(struct amigamesa_context *c);
void SWFSDriver_SwapBuffer_DR(struct amigamesa_context *c);
void Common_DD_pointers(GLcontext *ctx);

GLboolean SWFSDriver_set_buffer( GLcontext *ctx, GLenum mode );
void SWFSDriver_resize( GLcontext *ctx,GLuint *width, GLuint *height);
void SWFSDriver_flush(GLcontext *ctx);
void SWFSD_clear_index_LUT8(GLcontext *ctx, GLuint index);
void SWFSD_clear_index_RGB(GLcontext *ctx, GLuint index);
void SWFSD_set_index_LUT8(GLcontext *ctx, GLuint index);
void SWFSD_set_index_RGB(GLcontext *ctx, GLuint index);
void SWFSD_clear_color_LUT8(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_clear_color_ARGB(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_LUT8(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGB15(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGB15PC(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGB16(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGB16PC(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_BGR15PC(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_BGR16PC(GLcontext *ctx,
                            GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGB(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_BGR(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_ARGB(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_BGRA(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
void SWFSD_set_color_RGBA(GLcontext *ctx,
                                  GLubyte r, GLubyte g, GLubyte b, GLubyte a );
GLbitfield SWFSD_clear_LUT8(GLcontext *ctx, GLbitfield mask,
                      GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGB15(GLcontext *ctx, GLbitfield mask,
                        GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGB15PC(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGB16(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGB16PC(GLcontext *ctx, GLbitfield mask,
                        GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_BGR15PC(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_BGR16PC(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGB(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_BGR(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_ARGB(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_BGRA(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
GLbitfield SWFSD_clear_RGBA(GLcontext *ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height );
void SWFSD_write_color_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_write_color_span3_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_color_span3_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte rgba[][3], const GLubyte mask[]);
void SWFSD_write_monoci_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte mask[]);
void SWFSD_write_monorgba_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte mask[]);
void SWFSD_write_mono_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte mask[]);
void SWFSD_write_mono_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte mask[]);
void SWFSD_write_color_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_RGB15(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_RGB15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_RGB16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_color_pixels_BGRA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte rgba[][4], const GLubyte mask[] );
void SWFSD_write_monoci_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte mask[] );
void SWFSD_write_monorgba_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte mask[] );
void SWFSD_write_mono_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte mask[] );
void SWFSD_write_mono_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLubyte mask[] );
void SWFSD_write_index_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLubyte index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_span32_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLuint index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_span32_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLuint index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_span32_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        const GLuint index[],
                                        const GLubyte mask[]);
void SWFSD_write_index_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLuint index[],
                                              const GLubyte mask[] );
void SWFSD_write_index_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLuint index[],
                                              const GLubyte mask[] );
void SWFSD_write_index_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                              const GLuint index[],
                                              const GLubyte mask[] );
void SWFSD_read_color_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_RGB15(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_RGB15PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_color_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLubyte rgba[][4]);
void SWFSD_read_index_span_LUT8(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLuint index[]);
void SWFSD_read_index_span_RGB16(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLuint index[]);
void SWFSD_read_index_span_RGB16PC(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLuint index[]);
void SWFSD_read_index_span_ARGB(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLuint index[]);
void SWFSD_read_index_span_BGRA(const GLcontext *ctx, GLuint n, GLint x, GLint y,
                                        GLuint index[]);
void SWFSD_read_color_pixels_LUT8(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_RGB15(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_RGB15PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_RGB16(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_RGB16PC(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_ARGB(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_color_pixels_BGRA(const GLcontext *ctx, GLuint n, const GLint x[], const GLint y[],
                                        GLubyte rgba[][4], const GLubyte mask[]);
void SWFSD_read_index_pixels_LUT8(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                        GLuint index[], const GLubyte mask[] );
void SWFSD_read_index_pixels_RGB16(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLuint index[], const GLubyte mask[] );
void SWFSD_read_index_pixels_RGB16PC(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLuint index[], const GLubyte mask[] );
void SWFSD_read_index_pixels_ARGB(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLuint index[], const GLubyte mask[] );
void SWFSD_read_index_pixels_BGRA(const GLcontext *ctx,GLuint n, const GLint x[], const GLint y[],
                                         GLuint index[], const GLubyte mask[] );
#endif
