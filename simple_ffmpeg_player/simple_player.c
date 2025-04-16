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

	// ��ʼ��libavformat(���и�ʽ)��ע�����и�����/�⸴����
   // av_register_all();   // �ѱ�����Ϊ��ʱ�ģ�ֱ�Ӳ���ʹ�ü���

   // A1. ����Ƶ�ļ�����ȡ�ļ�ͷ�����ļ���ʽ��Ϣ�洢��"fmt context"��
	ret = avformat_open_input(&p_fmt_ctx, argv[1], NULL, NULL);
	if (ret < 0) {
		printf("avformat_open_input() failed\n");
		return -1;
	}

	// A2. ��������Ϣ����ȡһ����Ƶ�ļ����ݣ����Խ��룬��ȡ��������Ϣ����pFormatCtx->streams
	//     p_fmt_ctx->streams��һ��ָ�����飬�����С��pFormatCtx->nb_streams

	ret = avformat_find_stream_info(p_fmt_ctx, NULL);
	if (ret < 0) {
		printf("avformat_find_stream_info() failed\n");
		return -1;
	}

	// ���ļ������Ϣ��ӡ�ڱ�׼�����豸��
	av_dump_format(p_fmt_ctx, 0, argv[1], 0);
	// A3. ���ҵ�һ����Ƶ��
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
	// A5. Ϊ��Ƶ������������AVCodecContext
	// A5.1 ��ȡ����������AVCodecParameters
	p_codec_par = p_fmt_ctx->streams[v_idx]->codecpar;
	// A5.2 ��ȡ������
	p_codec = avcodec_find_decoder(p_codec_par->codec_id);
	if (p_codec == NULL) {
		printf("avcodec_find_decoder() failed\n");
		return -1;
	}
	// A5.3 ����������AVCodecContext
	// A5.3.1 p_codec_ctx��ʼ��������ṹ�壬ʹ��p_codec��ʼ����Ӧ��ԱΪĬ��ֵ
	p_codec_ctx = avcodec_alloc_context3(p_codec);

	// A5.3.2 p_codec_ctx��ʼ����p_codec_par ==> p_codec_ctx����ʼ����Ӧ��Ա
	ret = avcodec_parameters_to_context()
	// A5.3.3 p_codec_ctx��ʼ����ʹ��p_codec��ʼ��p_codec_ctx����ʼ�����

	 // A6. ����AVFrame
	// A6.1 ����AVFrame�ṹ��ע�Ⲣ������data buffer(��AVFrame.*data[])


	// A6.2 ΪAVFrame.*data[]�ֹ����仺���������ڴ洢sws_scale()��Ŀ��֡��Ƶ����
	//     p_frm_raw��data_buffer��av_read_frame()���䣬��˲����ֹ�����
	//     p_frm_yuv��data_buffer�޴����䣬����ڴ˴��ֹ�����

	// buffer����Ϊp_frm_yuv����Ƶ���ݻ�����

	// ʹ�ø��������趨p_frm_yuv->data��p_frm_yuv->linesize

	// A7. ��ʼ��SWS context�����ں���ͼ��ת��
	//     �˴���6������ʹ�õ���FFmpeg�е����ظ�ʽ���ԱȲο�ע��B4
	//     FFmpeg�е����ظ�ʽAV_PIX_FMT_YUV420P��ӦSDL�е����ظ�ʽSDL_PIXELFORMAT_IYUV
	//     ��������õ�ͼ��Ĳ���SDL֧�֣�������ͼ��ת���Ļ���SDL���޷�������ʾͼ���
	//     ��������õ�ͼ����ܱ�SDL֧�֣��򲻱ؽ���ͼ��ת��
	//     ����Ϊ�˱����㣬ͳһת��ΪSDL֧�ֵĸ�ʽAV_PIX_FMT_YUV420P==>SDL_PIXELFORMAT_IYUV

	// B1. ��ʼ��SDL��ϵͳ��ȱʡ(�¼������ļ�IO���߳�)����Ƶ����Ƶ����ʱ��

	// B2. ����SDL���ڣ�SDL 2.0֧�ֶര��
	//     SDL_Window�����г���󵯳�����Ƶ���ڣ�ͬSDL 1.x�е�SDL_Surface

	// B3. ����SDL_Renderer
	//     SDL_Renderer����Ⱦ��

	// B4. ����SDL_Texture
	//     һ��SDL_Texture��Ӧһ֡YUV���ݣ�ͬSDL 1.x�е�SDL_Overlay
	//     �˴���2������ʹ�õ���SDL�е����ظ�ʽ���ԱȲο�ע��A7
	//     FFmpeg�е����ظ�ʽAV_PIX_FMT_YUV420P��ӦSDL�е����ظ�ʽSDL_PIXELFORMAT_IYUV

	// A8. ����Ƶ�ļ��ж�ȡһ��packet
	//     packet��������Ƶ֡����Ƶ֡���������ݣ�������ֻ�������Ƶ֡����Ƶ֡��������Ƶ���ݲ����ᱻ
	//     �ӵ����Ӷ�����������ṩ�����ܶ����Ϣ
	//     ������Ƶ��˵��һ��packetֻ����һ��frame
	//     ������Ƶ��˵������֡���̶��ĸ�ʽ��һ��packet�ɰ���������frame��
	//                   ����֡���ɱ�ĸ�ʽ��һ��packetֻ����һ��frame

	// A9. ��Ƶ���룺packet ==> frame
	// A9.1 �������ι���ݣ�һ��packet������һ����Ƶ֡������Ƶ֡���˴���Ƶ֡�ѱ���һ���˵�

	// A9.2 ���ս�������������ݣ��˴�ֻ������Ƶ֡��ÿ�ν���һ��packet����֮����õ�һ��frame


	 // A10. ͼ��ת����p_frm_raw->data ==> p_frm_yuv->data
	 // ��Դͼ����һƬ���������򾭹��������µ�Ŀ��ͼ���Ӧ���򣬴����ͼ�����������������
	 // plane: ��YUV��Y��U��V����plane��RGB��R��G��B����plane
	 // slice: ͼ����һƬ�������У������������ģ�˳���ɶ������ײ����ɵײ�������
	 // stride/pitch: һ��ͼ����ռ���ֽ�����Stride=BytesPerPixel*Width+Padding��ע�����
	 // AVFrame.*data[]: ÿ������Ԫ��ָ���Ӧplane
   	 // AVFrame.linesize[]: ÿ������Ԫ�ر�ʾ��Ӧplane��һ��ͼ����ռ���ֽ���

	// B5. ʹ���µ�YUV�������ݸ���SDL_Rect

	// B6. ʹ���ض���ɫ��յ�ǰ��ȾĿ��
	
	// B7. ʹ�ò���ͼ������(texture)���µ�ǰ��ȾĿ��

	// B8. ִ����Ⱦ��������Ļ��ʾ

	// B9. ����֡��Ϊ25FPS���˴�����׼ȷ��δ���ǽ������ĵ�ʱ��
}



