//
// Created by Cesar Castelo on 15/10/2018.
//

#include <ift.h>

/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
	char program_description[2048] = "Trains a ConvNet Classifier.\n";

	iftCmdLineOpt cmd_line_opts[] = {
			{.short_name = "-i", .long_name = "--input", .has_arg=true, .arg_type=IFT_STR_TYPE,
					.required=true, .help="Input network architecture (.json file)."},
			{.short_name = "-it", .long_name = "--input_test", .has_arg=true, .arg_type=IFT_STR_TYPE,
					.required=true, .help="Input images for test (directory or csv file list)."},
			{.short_name = "-n", .long_name = "--epochs-number", .has_arg=true, .arg_type=IFT_LONG_TYPE,
					.required=false, .help="Number of training epochs (default = 100)."}
	};

	int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

	// Parser Setup
	iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
	iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
	iftDestroyCmdLineParser(&parser);

	return args;
}

void iftGetInputArgs(const iftDict *args, char** jsonPath, char** testPath, int* epochs) {

	*jsonPath = iftGetStrValFromDict("--input", args);

	*testPath = iftGetStrValFromDict("--input_test", args);

	if (iftDictContainKey("--epochs-number", args, NULL)) {
		*epochs = iftGetLongValFromDict("--epochs-number", args);
		if(*epochs <= 0) {
			iftExit("Invalid number of epochs %d (must be > 0).", "iftNeuralNet", *epochs);
		}
	} else {
		*epochs = 100;
	}
}

int main(int argc, const char** argv) {

	char *jsonPath, *testPath;
	int epochs;

	iftDict *args = iftGetArgs(argc, argv);
	iftGetInputArgs(args, &jsonPath, &testPath, &epochs);

	//iftPlatformInfo();

	iftRandomSeed(24);

	iftNeuralNetwork* net = iftCreateNeuralNetworkFromJson(jsonPath);
	net->verbose = true;

    printf("\n-> Network architecture ...\n");
	iftPrintNeuralNet(net);

    printf("\n-> Running gradient descent ...\n");
	timer *begin = iftTic();
    iftNeuralNetGradientDescent(net, epochs);
	timer* end = iftToc();

	printf("Training complete ");
	iftPrintFormattedTime(iftCompTime(begin, end));

	iftFileSet* filesetTest = iftLoadFileSetFromDirOrCSV(testPath, 1, true);
	begin = iftTic();
    iftTensor* pred = iftNeuralNetClassifyImageSet(net, filesetTest, IFT_TEST);
	end = iftToc();

    iftDataSet* Z2 = iftCreateDataSet(filesetTest->n, 1);
    int nLabels = net->label->dimension[1];
    for(int i = 0; i < filesetTest->n; i++) {
        iftUpdateDatasetFileInfo(filesetTest->files[i]);
        Z2->sample[i].truelabel = filesetTest->files[i]->label;

        float *prob = iftAllocFloatArray(nLabels);
        for(int c = 0; c < nLabels; c++)
            prob[c] = iftTensorElem(pred, i, c);

        Z2->sample[i].label = iftFArgmax(prob, nLabels);
        iftSetSampleStatus(&Z2->sample[i], IFT_TEST);
        iftFree(prob);
    }

	printf("Test score = %f ", iftTruePositives(Z2)); iftPrintFormattedTime(iftCompTime(begin, end));

	iftDestroyNeuralNetwork(&net);
	iftDestroyDataSet(&Z2);
    iftDestroyFileSet(&filesetTest);
    iftDestroyTensor(&pred);
}
