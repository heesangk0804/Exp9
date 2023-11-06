#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <math.h>
#include <stdio.h>
#include <float.h>


void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size);

int main()
{
 
	int height;
	int width;
	int channel;

    float PSNR_up,PSNR_bilinear;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("libcamera-still --width 960 --height 720 -o src.bmp");

 	unsigned char* imgIn = stbi_load("src.bmp", &width, &height, &channel, 3);
	unsigned char* imgOut_down0 = (unsigned char*) malloc (sizeof(unsigned char)*3*320*240);
	unsigned char* imgOut_up0 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
	unsigned char* imgOut_up1 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
 	
	downsampling(imgIn, height, width, channel, imgOut_down0);
	upsampling(imgOut_down0, height, width, channel, imgOut_up0);
	bilinear(imgOut_down0, height, width, channel, imgOut_up1);
	PSNR_up = imagePSNR(imgIn,imgOut_up0, 3*960*720);
	PSNR_bilinear = imagePSNR(imgIn,imgOut_up1, 3*960*720);
    	printf("======PSNR RESULT======\n");
	printf("%f, %f\n",PSNR_up,PSNR_bilinear);
	printf("=======================\n");
				
	stbi_write_bmp("image_down.bmp", width/3, height/3, channel, imgOut_down0);
	stbi_write_bmp("image_up.bmp", width, height, channel, imgOut_up0);
	stbi_write_bmp("image_bi.bmp", width, height, channel, imgOut_up1);
	
	stbi_image_free(imgIn);
	free(imgOut_down0);
	free(imgOut_up0);
	free(imgOut_up1);
 

	return 0;
}

void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {  
	int h, w, c;
	for (h = 0; h < height / 3; h++) 
	for (w = 0; w < width / 3; w++) 
	for (c = 0; c < channel; c++) 
		out[h * ( width / 3 ) * channel + w * channel + c] 
			= in[3 * h * width * channel + 3 * w * channel + c];
}

void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) { 
    int h, w, c;
	for (h = 0; h < height; h++) 
	for (w = 0; w < width; w++) 
	for (c = 0; c < channel; c++) 
		out[h * width * channel + w * channel + c] 
			= in[( h / 3 ) * ( width / 3 ) * channel + ( w / 3 ) * channel + c];
}

void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {  
    int h, w, c;
	for (h = 0; h < height - 3; h++) 
	for (w = 0; w < width - 3; w++) 
	for (c = 0; c < channel; c++) 
		out[h * width * channel + w * channel + c] 
			= ( ( 3 - w % 3 ) * ( 3 - h % 3 ) * in[(   h / 3   ) * ( width / 3 ) * channel + (   w / 3   ) * channel + c]
			+   (   w % 3   ) * ( 3 - h % 3 ) * in[(   h / 3   ) * ( width / 3 ) * channel + ( w / 3 + 1 ) * channel + c]
			+   ( 3 - w % 3 ) * (   h % 3   ) * in[( h / 3 + 1 ) * ( width / 3 ) * channel + (   w / 3   ) * channel + c]
			+   (   w % 3   ) * (   h % 3   ) * in[( h / 3 + 1 ) * ( width / 3 ) * channel + ( w / 3 + 1 ) * channel + c] ) / 9;	
	
	for(h = height - 3; h < height; h++)
	for(w = height - 3; w < width; w++)
	for(c = 0; c < channel; c++)
		out[h * width * channel + w * channel + c] 
			= in[( h / 3 ) * ( width / 3 ) * channel + ( w / 3 ) * channel + c];
		
}

//Calculates image PSNR value
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size){   
	double mse = 0;
	int i, maxi = 255;
	for(i = 0; i < size; i++)
		mse += ( frame1[i] - frame2[i] )**;
	mse /= size;
	return 20 * log10(maxi) - 10 * log10(mse);
}
