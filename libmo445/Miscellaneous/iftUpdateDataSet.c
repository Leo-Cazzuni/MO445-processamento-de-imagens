#include <ift.h>

int main(int argc, char** argv) {

	if (argc<3) {
        iftError("Usage: <old dataset> <new dataset>", "iftUpdateDataSet");
	}

	iftDataSet* old = iftReadOPFDataSet(argv[1]);
	iftWriteDataSet(old, argv[2]);

	return 0;
}
