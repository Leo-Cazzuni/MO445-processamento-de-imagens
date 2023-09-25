//
// Created by peixinho on 24/10/17.
//

#include <ift.h>

int main(int argc, char** argv) {

	if(argc<=2) {
		iftError("Usage: iftDataSetToCSV <input zip> <output csv> [<fileset txt>]", "iftDataSetToCSV");
	}

	printf("Read dataset\n");
	iftDataSet* dataset = iftReadDataSet(argv[1]);
	printf("Write CSV\n");
	if (argc==3)
	  iftWriteCSVDataSet(dataset, argv[2], NULL);
	else
	  iftWriteCSVDataSet(dataset, argv[2], argv[3]);
	iftDestroyDataSet(&dataset);

	return 0;
}
