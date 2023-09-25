//
// Created by Peixinho on 09/01/17.
//

#include <ift.h>

/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
	char program_description[2048] = "Trains an ConvNet Classifier from an image file set.\n";

	iftCmdLineOpt cmd_line_opts[] = {
			{.short_name = "-i", .long_name = "--input", .has_arg=true, .arg_type=IFT_STR_TYPE,
					.required=true, .help="Input images for training (directory or csv file list)."},
			//{.short_name = "-it", .long_name = "--input_test", .has_arg=true, .arg_type=IFT_STR_TYPE,
			//		.required=true, .help="Input images for test (directory or csv file list)."},
			{.short_name = "-s", .long_name = "--image-size", .has_arg=true, .arg_type=IFT_LONG_TYPE,
					.required=true, .help="Input image size."},
			{.short_name = "-t", .long_name = "--train-perc", .has_arg=true, .arg_type=IFT_DBL_TYPE,
					.required=true, .help="Train percentage for training (0,1.0)."},
			{.short_name = "-n", .long_name = "--epochs-number", .has_arg=true, .arg_type=IFT_LONG_TYPE,
					.required=false, .help="Number of training epochs (default = 100)."},
			{.short_name = "-a", .long_name = "--alpha", .has_arg=true, .arg_type=IFT_DBL_TYPE,
					.required=false, .help="Gradient Descent learning rate (default = 0.1)."},
			{.short_name = "-b", .long_name = "--minibatch", .has_arg=true, .arg_type=IFT_LONG_TYPE,
					.required=false, .help="Minibatch Gradient Descent  (default = 50)."}
	};

	int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

	// Parser Setup
	iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
	iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
	iftDestroyCmdLineParser(&parser);

	return args;
}

void iftGetInputArgs(const iftDict *args, char** trainpath, /*char** testpath, */int* size, float* perc, int* epochs, float* alpha, int *minibatch) {

	*trainpath = iftGetStrValFromDict("--input", args);

	//*testpath = iftGetStrValFromDict("--input_test", args);

	*size = iftGetLongValFromDict("--image-size", args);
	if(*size <= 0) {
        iftError("Invalid size %d (must be > 0).", "iftNeuralNet", *size);
	}


	*perc = iftGetDblValFromDict("--train-perc", args);
	if(*perc >= 1.0 || *perc <=0.0) {
        iftError("Invalid value for percentage %f (must be within (0, 1.0)).\n", "iftNeuralNet", *perc);
	}

	if (iftDictContainKey("--epochs-number", args, NULL)) {
		*epochs = iftGetLongValFromDict("--epochs-number", args);
		if(*epochs <= 0) {
            iftError("Invalid number of epochs %d (must be > 0).", "iftNeuralNet", *epochs);
		}
	} else {
		*epochs = 100;
	}

	if (iftDictContainKey("--minibatch", args, NULL)) {
		*minibatch = iftGetLongValFromDict("--minibatch", args);
		if(*minibatch <= 0) {
            iftError("Invalid minibatch size %d (must be > 0).", "iftNeuralNet", *minibatch);
		}
	} else {
		*minibatch = 50;
	}

	if (iftDictContainKey("--alpha", args, NULL)) {
		*alpha = iftGetDblValFromDict("--alpha", args);
		if(*alpha <= 0) {
            iftError("Invalid value for learning rate %f (must be > 0).", "iftNeuralNet", *alpha);
		}
	} else {
		*alpha = 0.1;
	}

	printf("Learning Neural Network for %s.\nTraining in %d epochs.\n", *trainpath, *epochs);
	printf("learning rate = %f.\n\n", *alpha);
}

int main(int argc, const char** argv) {

	char *filepath, *filepath_test;
	int minibatch, epochs, size;
	float trainperc, alpha;

	iftDataSet *Z=NULL;

	iftDict *args = iftGetArgs(argc, argv);
	iftGetInputArgs(args, &filepath, /*&filepath_test,*/ &size, &trainperc, &epochs, &alpha, &minibatch);

	iftPlatformInfo();

	iftRandomSeed(24);

	if(iftEndsWith(filepath, ".csv")) {
		Z = iftReadCSVDataSet(filepath, ' ', true, -1, false, 0);
	}
	else {
		Z = iftReadOPFDataSet(filepath);
	}

	//iftFileSet* fileset = iftLoadFileSetFromDirOrCSV(filepath, 0, false);
	//iftFileSet* fileset_test = iftLoadFileSetFromDirOrCSV(filepath, 0, false);


	iftIntArray* labels = iftGetDataSetTrueLabels(Z);

	iftSampler* sampler = iftStratifiedRandomSubsampling(labels->val, Z->nsamples, 1, trainperc * Z->nsamples);
	iftDataSetSampling(Z, sampler, 0);

	iftDestroyIntArray(&labels);

	iftDataSet* Z1 = iftExtractSamples(Z, IFT_TRAIN);
	iftDataSet* Z2 = iftExtractSamples(Z, IFT_TEST);

	iftNeuralNetwork* net = iftCreateNeuralNetwork(7, minibatch);
	net->verbose = true;

	int nlabels = Z->nclasses; //iftFileSetLabelsNumber(fileset);
	net->learnRate = alpha;

	net->layers[0] = iftDataSetLayer(net, Z1);
//	net->layers[1] = iftFullyConnectedLayer(net, 28*28, 1000, iftRelu, iftReluDeriv);
//	net->layers[2] = iftFullyConnectedLayer(net, 1000, 100, iftRelu, iftReluDeriv);
	net->layers[1] = iftConvolutionLayer2D(net, 28, 28, 1, 5, 5, 2, 2, 32, 0, iftRelu, iftReluDeriv);
	net->layers[2] = iftMaxPoolingLayer2D(net, 32, 12, 12, 2, 2);
	net->layers[3] = iftConvolutionLayer2D(net, 6, 6, 32, 3, 3, 1, 1, 128, 0, iftRelu, iftReluDeriv);
	net->layers[4] = iftMaxPoolingLayer2D(net, 128, 4, 4, 2, 2);
	net->layers[5] = iftFullyConnectedLayer(net, 128*2*2, nlabels, iftSigm, iftSigmDeriv);
	net->layers[6] = iftSquaredLossLayer(net, nlabels);

	iftPrintNeuralNet(net);

	timer *begin = iftTic();
    iftNeuralNetGradientDescent(net, epochs);
	timer* end = iftToc();


	printf("Training complete ");
	iftPrintFormattedTime(iftCompTime(begin, end));

	begin = iftTic();
	iftNeuralNetClassifyDataSet(net, Z2, IFT_TEST);
	end = iftToc();

	printf("Test score = %f ", iftTruePositives(Z2)); iftPrintFormattedTime(iftCompTime(begin, end));

	iftDestroyNeuralNetwork(&net);


	iftDestroySampler(&sampler);

	iftDestroyDataSet(&Z1);
	iftDestroyDataSet(&Z2);
	iftDestroyDataSet(&Z);
}
