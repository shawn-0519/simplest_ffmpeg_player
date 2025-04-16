#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <sdl/SDL.h>
#include <sdl/SDL_video.h>
#include <sdl/SDL_render.h>
#include <sdl/SDL_rect.h>

int main(int argc, char* argv[]) {

	// Initalizing these to NULL prevents segfaults!
	//ffmpeg
	AVFormatContext* p_fmt_ctx = NULL;
	AVCodecContext* p_codec_ctx = NULL;
	AVCodecParameters* p_codec_par = NULL;
	AVCodec* p_codec = NULL;
	AVFrame* p_frame = NULL;
	AVPacket* p_packet = NULL;
	struct SwsContext* sws_ctx = NULL;

	int buffer_size;
	uint8_t* buffer = NULL;
	int i;
	int v_idx;
	int ret;

	//SDL
	SDL_Window* p_window = NULL;
	SDL_Renderer* p_renderer = NULL;
	SDL_Texture* p_texture = NULL;
	SDL_Rect rect;
	SDL_Event event;
	
	if (argc < 2) {
		printf("please provide a movie file\n");
		return -1;
	}

	// 初始化libavformat(所有格式)，注册所有复用器/解复用器
   // av_register_all();   // 已被申明为过时的，直接不再使用即可

   // A1. 打开视频文件：读取文件头，将文件格式信息存储在"fmt context"中
	ret = avformat_open_input(&p_fmt_ctx, argv[1], NULL, NULL);
	if (ret < 0) {
		printf("avformat_open_input() failed\n");
		return -1;
	}

	// A2. 搜索流信息：读取一段视频文件数据，尝试解码，将取到的流信息填入pFormatCtx->streams
	//     p_fmt_ctx->streams是一个指针数组，数组大小是pFormatCtx->nb_streams

	ret = avformat_find_stream_info(p_fmt_ctx, NULL);
	if (ret < 0) {
		printf("avformat_find_stream_info() failed\n");
		return -1;
	}

	// 将文件相关信息打印在标准错误设备上
	av_dump_format(p_fmt_ctx, 0, argv[1], 0);
	// A3. 查找第一个视频流
	v_idx = -1;
	for (i = 0; i < p_fmt_ctx->nb_streams; i++) {
		if (p_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			v_idx = i;
			printf("Find a video stream, index %d\n", v_idx);
			break;
		}
	}

	if (v_idx == -1) {
		printf("No video stream found\n");
		return -1;
	}
	// A5. 为视频流构建解码器AVCodecContext
	// A5.1 获取解码器参数AVCodecParameters
	p_codec_par = p_fmt_ctx->streams[v_idx]->codecpar;
	// A5.2 获取解码器
	p_codec = avcodec_find_decoder(p_codec_par->codec_id);
	if (p_codec == NULL) {
		printf("avcodec_find_decoder() failed\n");
		return -1;
	}
	// A5.3 构建解码器AVCodecContext
	// A5.3.1 p_codec_ctx初始化：分配结构体，使用p_codec初始化相应成员为默认值
	p_codec_ctx = avcodec_alloc_context3(p_codec);

	// A5.3.2 p_codec_ctx初始化：p_codec_par ==> p_codec_ctx，初始化相应成员
	ret = avcodec_parameters_to_context()
	// A5.3.3 p_codec_ctx初始化：使用p_codec初始化p_codec_ctx，初始化完成

	 // A6. 分配AVFrame
	// A6.1 分配AVFrame结构，注意并不分配data buffer(即AVFrame.*data[])


	// A6.2 为AVFrame.*data[]手工分配缓冲区，用于存储sws_scale()中目的帧视频数据
	//     p_frm_raw的data_buffer由av_read_frame()分配，因此不需手工分配
	//     p_frm_yuv的data_buffer无处分配，因此在此处手工分配

	// buffer将作为p_frm_yuv的视频数据缓冲区

	// 使用给定参数设定p_frm_yuv->data和p_frm_yuv->linesize

	// A7. 初始化SWS context，用于后续图像转换
	//     此处第6个参数使用的是FFmpeg中的像素格式，对比参考注释B4
	//     FFmpeg中的像素格式AV_PIX_FMT_YUV420P对应SDL中的像素格式SDL_PIXELFORMAT_IYUV
	//     如果解码后得到图像的不被SDL支持，不进行图像转换的话，SDL是无法正常显示图像的
	//     如果解码后得到图像的能被SDL支持，则不必进行图像转换
	//     这里为了编码简便，统一转换为SDL支持的格式AV_PIX_FMT_YUV420P==>SDL_PIXELFORMAT_IYUV

	// B1. 初始化SDL子系统：缺省(事件处理、文件IO、线程)、视频、音频、定时器

	// B2. 创建SDL窗口，SDL 2.0支持多窗口
	//     SDL_Window即运行程序后弹出的视频窗口，同SDL 1.x中的SDL_Surface

	// B3. 创建SDL_Renderer
	//     SDL_Renderer：渲染器

	// B4. 创建SDL_Texture
	//     一个SDL_Texture对应一帧YUV数据，同SDL 1.x中的SDL_Overlay
	//     此处第2个参数使用的是SDL中的像素格式，对比参考注释A7
	//     FFmpeg中的像素格式AV_PIX_FMT_YUV420P对应SDL中的像素格式SDL_PIXELFORMAT_IYUV

	// A8. 从视频文件中读取一个packet
	//     packet可能是视频帧、音频帧或其他数据，解码器只会解码视频帧或音频帧，非音视频数据并不会被
	//     扔掉、从而能向解码器提供尽可能多的信息
	//     对于视频来说，一个packet只包含一个frame
	//     对于音频来说，若是帧长固定的格式则一个packet可包含整数个frame，
	//                   若是帧长可变的格式则一个packet只包含一个frame

	// A9. 视频解码：packet ==> frame
	// A9.1 向解码器喂数据，一个packet可能是一个视频帧或多个音频帧，此处音频帧已被上一句滤掉

	// A9.2 接收解码器输出的数据，此处只处理视频帧，每次接收一个packet，将之解码得到一个frame


	 // A10. 图像转换：p_frm_raw->data ==> p_frm_yuv->data
	 // 将源图像中一片连续的区域经过处理后更新到目标图像对应区域，处理的图像区域必须逐行连续
	 // plane: 如YUV有Y、U、V三个plane，RGB有R、G、B三个plane
	 // slice: 图像中一片连续的行，必须是连续的，顺序由顶部到底部或由底部到顶部
	 // stride/pitch: 一行图像所占的字节数，Stride=BytesPerPixel*Width+Padding，注意对齐
	 // AVFrame.*data[]: 每个数组元素指向对应plane
   	 // AVFrame.linesize[]: 每个数组元素表示对应plane中一行图像所占的字节数

	// B5. 使用新的YUV像素数据更新SDL_Rect

	// B6. 使用特定颜色清空当前渲染目标
	
	// B7. 使用部分图像数据(texture)更新当前渲染目标

	// B8. 执行渲染，更新屏幕显示

	// B9. 控制帧率为25FPS，此处不够准确，未考虑解码消耗的时间
}



