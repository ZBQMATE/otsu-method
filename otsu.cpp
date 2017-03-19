#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <windows.h>

using namespace std;

int otsu() {
	
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	
	FILE *pic = fopen("./h.bmp", "rb");
	FILE *picout = fopen("./otsu_segmentation.bmp", "wb");
	
	fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, pic);
	fread(&infoheader, sizeof(BITMAPINFOHEADER), 1, pic);
	
	int height = infoheader.biHeight;
	int width = infoheader.biWidth;
	int picsize = height * width;
	
	unsigned char img[height*width*4];
	
	for (int i = 0; i < height*width*4; i++) {
		img[i] = 0;
	}
	
	//otsu transfer
	
	//read input image
	unsigned char input_img[height*width*3];
	fread(&input_img, height*width*3, 1, pic);
	
	//histogram
	int histogram[256];
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}
	
	//histogram on blue scale
	int a = 0;
	for (int i = 0; i < height * width; i++) {
		a = input_img[i * 3];
		histogram[a] = histogram[a] + 1;
	}
	
	//histogram to its propotional percentage
	double pct_histogram[256];
	for (int i = 0; i < 256; i++) {
		pct_histogram[i] = (double) histogram[i] / (height*width);
	}
	
	//optimal sepretion using otsu method
	
	int opt_thre = 0;
	double opt_var = 0.0;
	double cur_var = 0.0;
	
	double pre_var = 0.0;
	double pos_var = 0.0;
	
	double pre_pro = 0.0;
	double pos_pro = 0.0;
	
	
	for (int t = 1; t < 255; t++) {
		
		for (int st = 0; st < t; st++) {
			pre_pro = pre_pro + pct_histogram[st];
		}
		
		for (int st = 0; st < t; st++) {
			pre_var = pre_var+(pct_histogram[st]-(pre_pro/t))*(pct_histogram[st]-(pre_pro/t));
		}
		pre_var = pre_var / t;
		
		
		for (int lt = t; lt < 256; lt++) {
			pos_pro = pos_pro + pct_histogram[lt];
		}
		
		for (int lt = t; lt < 256; lt++) {
			pos_var = pos_var+(pct_histogram[lt]-pos_pro/(256-t))*(pct_histogram[lt]-pos_pro/(256-t));
		}
		pos_var = pos_var / (256 - t);
		
		
		cur_var = pre_pro * pre_var + pos_pro * pos_var;
		
		if (t == 1) {
			opt_var = cur_var;
			opt_thre = t;
		}
		
		if (cur_var < opt_var) {
			opt_var = cur_var;
			opt_thre = t;
		}
		
		pre_pro = 0.0;
		pre_var = 0.0;
		
		pos_pro = 0.0;
		pos_var = 0.0;
		//opt_thre = 255;
	}
	
	
	//devide the image
	
	for (int s = 0; s < height * width; s++) {
		
		if (input_img[s * 3] > opt_thre) {
			img[s * 4] = 255;
		}
		
		if (input_img[s * 3] == opt_thre) {
			img[s * 4] = 120;
		}
		
		if (input_img[s * 3] < opt_thre) {
			img[s * 4] = 0;
		}
		
	}
	
	
	//save
	
	infoheader.biBitCount = 32;
	
	fwrite(&fileheader, sizeof(BITMAPFILEHEADER), 1, picout);
	fwrite(&infoheader, sizeof(BITMAPINFOHEADER), 1, picout);
	fwrite(img, picsize, 4, picout);
	
	fclose(pic);
	fclose(picout);
	
	return 0;
}

int main() {
	otsu();
}