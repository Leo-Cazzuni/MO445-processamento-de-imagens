//
// Created by peixinho on 19/06/17.
//

#include <ift.h>
#include <iftDeepLearning.cuh>
#include <sys/time.h>

double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

iftTensor* iftImageToTensor(iftImage* img) {

    iftTensor* T = iftCreateTensor(4, 1, iftIsColorImage(img)? 3: 1, img->xsize, img->ysize);

    iftPrintIntArray(T->dimension, T->ndimension);

    for (int i = 0; i < img->xsize; ++i) {
        for (int j = 0; j < img->ysize; ++j) {

//            iftColor ycbcr;
//
//            ycbcr.val[0] = iftImgVal2D(img, i, j);
//            ycbcr.val[1] = iftImgCb2D(img, i, j);
//            ycbcr.val[2] = iftImgCr2D(img, i, j);
//
//
//            iftColor rgb = iftYCbCrtoRGB(ycbcr, 255);
//
//            iftTensorElem(T, 0, 0, i, j) = rgb.val[0];
//            iftTensorElem(T, 0, 1, i, j) = rgb.val[1];
//            iftTensorElem(T, 0, 2, i, j) = rgb.val[2];

            iftTensorElem(T, 0, 0, i, j) = iftImgVal2D(img, i, j);

        }
    }

    return T;
}

iftTensor* iftKernelToTensor(int bands, int width, int height, iftKernel* k1, iftKernel* k2, iftKernel* k3) {
    iftTensor* T = iftCreateTensor(4, 3, bands, width, height);

    for (int b = 0; b < bands; ++b) {
        int k = 0;
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                iftTensorElem(T, 0, b, i, j) = k1->weight[k];
                iftTensorElem(T, 1, b, i, j) = k2->weight[k];
                iftTensorElem(T, 2, b, i, j) = k3->weight[k];

                k++;
            }
        }
    }

    return T;
}

void iftReferenceImages(iftImage* img, iftKernel* k1, iftKernel* k2, iftKernel* k3) {


    iftImage* img1 = iftLinearFilter(img, k1);
    iftWriteImageByExt(img1, "def0.pgm");

    img1 = iftLinearFilter(img, k2);
    iftWriteImageByExt(img1, "def1.pgm");

    img1 = iftLinearFilter(img, k3);
    iftWriteImageByExt(img1, "def2.pgm");

}

void iftWriteImagesTensor(iftTensor* imgs) {
    iftImage* img = iftCreateImage(imgs->dimension[2], imgs->dimension[3], 1);

    iftPrintIntArray(imgs->dimension, imgs->ndimension);

    for (int i = 0; i < img->xsize; ++i) {
        for (int j = 0; j < img->ysize; ++j) {
            iftImgVal2D(img, i, j) = iftTensorElem(imgs, 0, 0, i, j);
        }
    }

    iftWriteImageByExt(img, "img0.pgm");

    printf("img0\n");

    iftPrintIntArray(imgs->dimension, imgs->ndimension);

    for (int i = 0; i < img->xsize; ++i) {
        for (int j = 0; j < img->ysize; ++j) {
            iftImgVal2D(img, i, j) = iftTensorElem(imgs, 0, 1, i, j);
        }
    }


    iftWriteImageByExt(img, "img1.pgm");
    printf("img1\n");


    for (int i = 0; i < img->xsize; ++i) {
        for (int j = 0; j < img->ysize; ++j) {
            iftImgVal2D(img, i, j) = iftTensorElem(imgs, 0, 2, i, j);
        }
    }

    iftWriteImageByExt(img, "img2.pgm");
    printf("img2\n");

}

int main() {

//    iftTensor* data = iftCreateTensor(4, 1, 1, 10, 10);
//    iftTensor* kernel = iftCreateTensor(4, 1, 1, 5, 5);
//
//    iftTensor* out = iftCreateTensor(4, 1, 1, 6, 6);
//
//    iftSetFloatArray(data->val, data->n, 1.0);
//    iftSetFloatArray(kernel->val, kernel->n, 1.0);
//
//    iftNeuralConvolution(data, kernel, out);
//
//    printf("Done\n");
//
//
//    iftPrintFloatArray(out->val, out->n);

	double t_start, t_end;

    printf("Main\n");

    iftTensor* data = iftReadTensorCsv("/home/barbara/Documents/ift/C.csv","/home/barbara/Documents/ift/C.dim.csv",' ');
	printf("Leu 1\n");
	iftTensor* kernel = iftReadTensorCsv("/home/barbara/Documents/ift/W.csv","/home/barbara/Documents/ift/W.dim.csv", ' ');
	printf("Leu 2\n");
	iftTensor* out = iftReadTensorCsv("/home/barbara/Documents/ift/X.csv","/home/barbara/Documents/ift/X.dim.csv", ' ');
	printf("Leu 3\n");


/*
	for(int x=0; x<data->dimension[2]; ++x) {
		for(int y=0; y<data->dimension[3]; ++y) {
			printf("%f ", iftTensorElem(data, 0, 0, x, y));	
		}
		printf("\n");
	}*/

	iftTensor* out_copy = iftReadTensorCsv("/home/barbara/Documents/ift/out.csv","/home/barbara/Documents/ift/out.dim.csv", ' ');
	printf("Cria copia\n");

	for(int i = 0; i < out_copy->n; i++){
		printf("f: %f \n", out_copy->val[i]);
	}

	iftTensor* out2; 
	
	iftStartGPU();
	t_start = rtclock();
    //iftNeuralConvolution(data, kernel, out);
	out2 = iftNeuralConvolutionUpdate(data, out, kernel->dimension);
	//iftNeuralConvolutionGPU(data, kernel, out);
	t_end = rtclock();

    printf("Rodou os roles\n");

	

	for(int i = 0; i < out2->n; i++){
		printf("f: %f \n", out2->val[i]);
	}

	float diff, sum;

    for(int i = 0; i < out2->n; i++){
		diff = 0.0;
		diff = fabs(out2->val[i] - out_copy->val[i]);
		if (diff >= 0.001){
			sum = sum + diff;			
			printf("i: %d diff: %f\n", i, diff);
		}
	}
	
	printf("\n");
	
	fprintf(stdout, "\n%0.6lfs\n", t_end - t_start);  

}
